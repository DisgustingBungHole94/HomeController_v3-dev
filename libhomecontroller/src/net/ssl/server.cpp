#include "homecontroller/net/ssl/server.h"

#include "homecontroller/exception.h"
#include "homecontroller/util/logger.h"

#include <sys/socket.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <set>

namespace hc {
namespace net {
namespace ssl {

    void server::init(int port, const std::string& cert_file, const std::string& priv_key_file, int default_timeout) {
        m_default_timeout = default_timeout;

        m_ssl_ctx.reset(SSL_CTX_new(TLS_server_method()));
        if (m_ssl_ctx == nullptr) {
            throw exception("failed to create ssl context", "hc::net::ssl::server::init");
        }

        SSL_CTX_set_min_proto_version(m_ssl_ctx.get(), TLS1_2_VERSION);

        if (SSL_CTX_use_certificate_file(m_ssl_ctx.get(), cert_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
            util::logger::err("openssl error: " + error_str());
            throw exception("failed to load specified certificate file", "hc::net::ssl::server::init");
        }

        if (SSL_CTX_use_PrivateKey_file(m_ssl_ctx.get(), priv_key_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
            util::logger::err("openssl error: " + error_str());
            throw exception("failed to load specified private key file", "hc::net::ssl::server::init");
        }

        m_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_socket < 0) {
            throw exception("failed to create server socket", "hc::net::ssl::server::init");
        }

        const int reuse_addr = 1;
        setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(int));

        sockaddr_in server_addr = {};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(m_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            throw exception("failed to bind socket, is the port in use?", "hc::net::ssl::server::init");
        }

        bool err = false;
        std::string err_msg;

        if ((m_epfd = epoll_create(1)) == -1) {
            err = true;
            err_msg = "failed to create epoll fd";
        } else if (!set_nonblocking(m_socket)) {
            err = true;
            err_msg = "failed to set server socket to nonblocking";
        } else if (!epoll_ctl_add(m_socket, EPOLLIN | EPOLLOUT, nullptr)) {
            err = true;
            err_msg = "failed to add server socket to epoll list";
        } else if (!init_close_pipe()) {
            err = true;
            err_msg = "failed to create pipe for server close detection";
        }

        if (err) {
            if (m_socket != -1)         ::close(m_socket);
            if (m_epfd != -1)           ::close(m_epfd);
            
            throw exception(err_msg, "hc::net::ssl::server::init");
        }

        listen(m_socket, 16);

        m_running = true;

        util::logger::log("server listening on port " + std::to_string(port));
    }

    void server::run(int max_conns) {
        static const int MAX_EVENTS = max_conns;
        epoll_event events[MAX_EVENTS];
        
        do {
            int num_fds = epoll_wait(m_epfd, events, MAX_EVENTS, -1);

            for (int i = 0; i < num_fds; i++) {
                epoll_data* data_ptr = static_cast<epoll_data*>(events[i].data.ptr);

                // server closed
                if (data_ptr->m_fd == m_close_fd_r) {
                    if (!m_running) {
                        break;
                    }

                    //int fd;
                    //read(m_close_fd_r, &fd, sizeof(fd));
                }

                // new connection
                else if (data_ptr->m_fd == m_socket) {
                    accept();
                } 
                
                else {
                    // connection closed
                    if (events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
                        handle_close(data_ptr);
                    }

                    // data received
                    else if (events[i].events & EPOLLIN) {
                        // if fd matches socket, data was received on socket
                        if (data_ptr->m_fd == data_ptr->m_conn_data->m_socket_fd) {
                            handle_data(data_ptr);
                        }
                        
                        // otherwise, data was received from timer fd, socket timed out
                        else if (data_ptr->m_fd == data_ptr->m_conn_data->m_timer_fd) {
                            handle_timeout(data_ptr);
                        }
                    }
                }
            }
        } while(m_running);

        // close server socket
        shutdown(m_socket, SHUT_RDWR);
        ::close(m_socket);

        // close epoll fd
        ::close(m_epfd);

        // close client fds
        int num_closed = 0;
        for (auto& x : m_epoll_fds) {
            if (x.second->m_conn_data != nullptr && !x.second->m_conn_data->m_conn_ptr->is_closed() && x.second->m_conn_data->m_conn_ptr->is_ready()) {
                x.second->m_conn_data->m_conn_ptr->close();
                m_on_disconnect_callback(x.second->m_conn_data->m_conn_ptr);
                num_closed++;
            }

            delete x.second;
        }

        m_epoll_fds.clear();

        util::logger::dbg("disconnected " + std::to_string(num_closed) + " clients");
        util::logger::log("server closed");
    }

    void server::stop() {
        m_running = false;
        write(m_close_fd_w, "A", 1); // wake up epoll
    }

    void server::close_connection(server_conn_ptr conn_ptr) {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);

        auto mit = m_epoll_fds.find(conn_ptr->get_socket());
        if (mit == m_epoll_fds.end()) {
            if (conn_ptr->is_closed()) {
                // connection was likely closed by another thread
                return;
            }

            throw exception("connection not found in epoll list", "hc::net::ssl::server::close_connection");
        }

        util::logger::dbg("disconnected client [" + conn_ptr->get_ip() + "]");

        remove_epoll_data(mit->second);

        //int fd = conn_ptr->get_socket();
        //write(m_close_fd_r, &fd, sizeof(fd));
    }

    void server::toggle_timeout(server_conn_ptr conn_ptr) {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);

        auto mit = m_epoll_fds.find(conn_ptr->get_socket());
        if (mit == m_epoll_fds.end()) {
            throw exception("connection not found in epoll list", "hc::net::ssl::server::close_connection");
        }

        mit->second->m_conn_data->m_timeout_enabled = !mit->second->m_conn_data->m_timeout_enabled;

        int new_time = 0;
        if (mit->second->m_conn_data->m_timeout_enabled) {
            new_time = m_default_timeout;
        }

        if (!update_timer_fd(mit->second->m_conn_data->m_timer_fd, new_time)) {
            util::logger::err("failed to toggle timeout, failed to update timer fd");
        }

        util::logger::dbg("toggled timeout for client [" + conn_ptr->get_ip() + "]");
    }

    /*void server::on_connect(server_conn_hdl conn_hdl) {}
    void server::on_data(server_conn_hdl conn_hdl) {}
    void server::on_disconnect(server_conn_hdl conn_hdl) {}*/

    void server::accept() {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);

        sockaddr_in client_addr;
        int client_len = sizeof(client_addr);

        int client_socket = ::accept(m_socket, (sockaddr*)&client_addr, (socklen_t*)&client_len);
        if (client_socket == -1) {
            util::logger::err("failed to establish connection with client");
            return;
        }

        unique_ptr<SSL> client_ssl(SSL_new(m_ssl_ctx.get()));
        SSL_set_fd(client_ssl.get(), client_socket);

        server_conn_ptr client_conn = std::make_shared<server_connection>(std::move(client_ssl));
        client_conn->set_ip(get_socket_ip(&client_addr));

        bool err = false;
        int timer_fd;

        if ((timer_fd = create_timer_fd()) == -1) {
            err = true;
            util::logger::err("failed to create timer for client");
        } else if (!set_nonblocking(timer_fd) || !set_nonblocking(client_socket)) {
            err = true;
            util::logger::err("failed to make fds nonblocking");
        }

        if (err) {
            if (timer_fd != -1) ::close(timer_fd);
            if (client_socket != -1) :: close(client_socket);

            return;
        }

        std::shared_ptr<connection_data> client_data = std::make_shared<connection_data>();
        client_data->m_socket_fd = client_socket;
        client_data->m_timer_fd = timer_fd;
        client_data->m_conn_ptr = client_conn;
        
        if (!epoll_ctl_add(client_socket, (EPOLLIN | EPOLLRDHUP | EPOLLHUP), client_data)
         || !epoll_ctl_add(timer_fd, EPOLLIN, client_data)) 
        {
            ::close(client_socket);
            ::close(timer_fd);

            util::logger::err("failed to add client fds to epoll list");
            return;
        }

        util::logger::dbg("client [" + client_conn->get_ip() + "] connected");
    }

    void server::handle_close(epoll_data* data_ptr) {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);

        util::logger::dbg("client [" + data_ptr->m_conn_data->m_conn_ptr->get_ip() + "] disconnected");
        remove_epoll_data(data_ptr);
    }

    void server::handle_data(epoll_data* data_ptr) {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);

        if (!update_timer_fd(data_ptr->m_conn_data->m_timer_fd, m_default_timeout)) {
            util::logger::err("failed to update timer for client");
        }

        if (!data_ptr->m_conn_data->m_conn_ptr->is_ready()) {
            if (!data_ptr->m_conn_data->m_conn_ptr->handshake()) {
                util::logger::err("client handshake failed: " + error_str());
                close_connection(data_ptr->m_conn_data->m_conn_ptr);
            } else {
                if (data_ptr->m_conn_data->m_conn_ptr->is_ready()) {
                    m_on_connect_callback(data_ptr->m_conn_data->m_conn_ptr);
                }
            }
        } else {
            m_on_data_callback(data_ptr->m_conn_data->m_conn_ptr);
        }
    }

    void server::handle_timeout(epoll_data* data_ptr) {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);

        if (data_ptr->m_conn_data->m_timeout_enabled) {
            util::logger::dbg("client [" + data_ptr->m_conn_data->m_conn_ptr->get_ip() + "] timed out");
            remove_epoll_data(data_ptr);
        }
    }

    bool server::validate_epoll_data(int fd) {
        return true;
    }

    void server::remove_epoll_data(epoll_data* data_ptr) {
        if (!epoll_ctl_del(data_ptr->m_conn_data->m_socket_fd))
            util::logger::err("failed to remove client socket from epoll list");
        if (!epoll_ctl_del(data_ptr->m_conn_data->m_timer_fd))
            util::logger::err("failed to remove client timer from epoll list");
        
        util::logger::dbg("client [" + data_ptr->m_conn_data->m_conn_ptr->get_ip() + "] connected");

        data_ptr->m_conn_data->m_conn_ptr->close();
        ::close(data_ptr->m_conn_data->m_timer_fd);

        m_on_disconnect_callback(data_ptr->m_conn_data->m_conn_ptr);

        delete data_ptr;
    }

    bool server::set_nonblocking(int fd) {
        if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) < 0) {
            return false;
        }

        return true;
    }

    bool server::epoll_ctl_add(int fd, uint32_t events, const std::shared_ptr<connection_data>& data_ptr) {
        epoll_data* epoll_data_ptr = new epoll_data();
        epoll_data_ptr->m_fd = fd;
        epoll_data_ptr->m_conn_data = data_ptr;

        epoll_event ev;
        ev.events = events;
        ev.data.ptr = epoll_data_ptr;

        if (epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {           
            return false;
        }

        m_epoll_fds.insert(std::make_pair(fd, epoll_data_ptr));

        return true;
    }

    bool server::epoll_ctl_del(int fd) {        
        if (epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
            return false;
        }

        m_epoll_fds.erase(fd);

        return true;
    }

    int server::create_timer_fd() {
        int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
        if (timer_fd == -1) {
            return -1;
        }

        if (!update_timer_fd(timer_fd, m_default_timeout)) {
            ::close(timer_fd);
            return -1;
        }
        

        return timer_fd;
    }

    bool server::update_timer_fd(int fd, int time) {
        itimerspec timeout;
        timeout.it_interval.tv_sec = 0;
        timeout.it_interval.tv_nsec = 0;
        timeout.it_value.tv_sec = time / 1000;
        timeout.it_value.tv_nsec = (time % 1000);

        if (timerfd_settime(fd, 0, &timeout, nullptr) == -1) {
            return false;
        }

        return true;
    }

    bool server::init_close_pipe() {
        int pipe_fds[2] = {};
        if (pipe(pipe_fds) == -1) {
            return false;
        }

        m_close_fd_r = pipe_fds[0];
        m_close_fd_w = pipe_fds[1];

        if (!set_nonblocking(m_close_fd_w)) {
            return false;
        }

        if (!epoll_ctl_add(m_close_fd_r, EPOLLIN, nullptr)) {
            return false;
        }

        return true;
    }

    std::string server::get_socket_ip(sockaddr_in* client_addr) {
        in_addr ip_addr = client_addr->sin_addr;
        char ip_buf[INET_ADDRSTRLEN];

        inet_ntop(AF_INET, &ip_addr, ip_buf, INET_ADDRSTRLEN);

        return std::string(ip_buf);
    }

}
}
}