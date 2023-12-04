#pragma once

#include "homecontroller/net/ssl/server_connection.h"

#include <netinet/in.h>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <set>
#include <map>

namespace hc {
namespace net {
namespace ssl {
    
    class server {
        public:
            server() 
                : m_socket(-1), m_running(false), m_default_timeout(-1)
            {}

            ~server() {}

            void init(int port, const std::string& cert_file, const std::string& priv_key_file, int default_timeout);

            void run(int max_conns);
            void stop();

            /*virtual void on_connect(server_conn_hdl conn_hdl);
            virtual void on_data(server_conn_hdl conn_hdl);
            virtual void on_disconnect(server_conn_hdl conn_hdl);*/

            void set_on_connect_callback(std::function<void(server_conn_ptr)> callback) { m_on_connect_callback = callback; }
            void set_on_data_callback(std::function<void(server_conn_ptr)> callback) { m_on_data_callback = callback; }
            void set_on_disconnect_callback(std::function<void(server_conn_ptr)> callback) { m_on_disconnect_callback = callback; }

            void close_connection(server_conn_ptr conn_ptr);
            void toggle_timeout(server_conn_ptr conn_ptr);

        private:
            struct connection_data {
                int m_socket_fd;
                int m_timer_fd;

                bool m_timeout_enabled = true;

                server_conn_ptr m_conn_ptr;
            };

            struct epoll_data {
                int m_fd;
                std::shared_ptr<connection_data> m_conn_data;
            };

            std::function<void(server_conn_ptr)> m_on_connect_callback;
            std::function<void(server_conn_ptr)> m_on_data_callback;
            std::function<void(server_conn_ptr)> m_on_disconnect_callback;

            void accept();

            void handle_close(epoll_data* data_ptr);
            void handle_data(epoll_data* data_ptr);
            void handle_timeout(epoll_data* data_ptr);

            bool validate_epoll_data(int fd);

            void remove_epoll_data(epoll_data* data_ptr);

            bool set_nonblocking(int fd);

            bool epoll_ctl_add(int fd, uint32_t events, const std::shared_ptr<connection_data>& data_ptr);
            bool epoll_ctl_del(int fd);

            int create_timer_fd();
            bool update_timer_fd(int fd, int time);

            bool init_close_pipe();

            std::string get_socket_ip(sockaddr_in* client_addr);

            unique_ptr<SSL_CTX> m_ssl_ctx;
            int m_socket;
            int m_epfd;

            std::unordered_map<int, epoll_data*> m_epoll_fds;

            int m_close_fd_r;
            int m_close_fd_w;

            std::map<int, epoll_data*> m_close_list;

            bool m_running;

            int m_default_timeout;

            // thread safety
            std::mutex m_mutex;
    };

}
}
}