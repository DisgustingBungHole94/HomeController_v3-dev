#include "homecontroller/net/ws/wrapper.h"

#include "homecontroller/exception.h"
#include "homecontroller/util/logger.h"

namespace hc {
namespace net {
namespace ws {
    
    /*template<typename config_type>
    void wrapper<config_type>::wrap(std::shared_ptr<ssl::connection> conn_ptr) {
        m_conn_ptr = conn_ptr;

        init_ws_conn();

        m_ws_conn_ptr->set_vector_write_handler(std::bind(&wrapper::on_vector_write, this, std::placeholders::_1, std::placeholders::_2));
        m_ws_conn_ptr->set_write_handler(std::bind(&wrapper::on_write, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        m_ws_conn_ptr->set_open_handler(std::bind(&wrapper::on_open, this, std::placeholders::_1));
        m_ws_conn_ptr->set_fail_handler(std::bind(&wrapper::on_fail, this, std::placeholders::_1));
        m_ws_conn_ptr->set_close_handler(std::bind(&wrapper::on_close, this, std::placeholders::_1));
        m_ws_conn_ptr->set_message_handler(std::bind(&wrapper::on_message, this, std::placeholders::_1, std::placeholders::_2));

        m_ws_conn_ptr->start();
    }*/

    template<typename config_type>
    void wrapper<config_type>::init() {
        init_ws_conn();

        m_ws_conn_ptr->set_vector_write_handler(std::bind(&wrapper::on_vector_write, this, std::placeholders::_1, std::placeholders::_2));
        m_ws_conn_ptr->set_write_handler(std::bind(&wrapper::on_write, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        m_ws_conn_ptr->set_open_handler(std::bind(&wrapper::on_open, this, std::placeholders::_1));
        m_ws_conn_ptr->set_fail_handler(std::bind(&wrapper::on_fail, this, std::placeholders::_1));
        m_ws_conn_ptr->set_close_handler(std::bind(&wrapper::on_close, this, std::placeholders::_1));
        m_ws_conn_ptr->set_message_handler(std::bind(&wrapper::on_message, this, std::placeholders::_1, std::placeholders::_2));

        m_ws_conn_ptr->start();
    }

    template<typename config_type>
    void wrapper<config_type>::init_ws_conn() {}

    template<typename config_type>
    void wrapper<config_type>::send(const std::string& data) {
        try {
            m_ws_conn_ptr->send(&data[0], data.size(), websocketpp::frame::opcode::BINARY);
        } catch(websocketpp::exception& e) {
            throw exception("websocket write failed: " + std::string(e.what()), "hc::net::ws::wrapper::write");
        }
    }

    /*template<typename config_type>
    void wrapper<config_type>::recv() {
        std::shared_ptr<ssl::connection> conn_ptr;
        if (!(conn_ptr = m_conn_hdl.lock())) {
            throw exception("bad connection pointer", "hc::net::ws::wrapper::recv");
        }

        std::string data = conn_ptr->recv();
        recv_external(data);
    }*/

    template<typename config_type>
    void wrapper<config_type>::recv() {
        if (m_tmp_conn_ptr == nullptr) {
            throw hc::exception("no connection being used", "hc::net::ws::wrapper::recv");
        }

        std::string data = m_tmp_conn_ptr->recv();

        m_message_log.clear();
        m_ws_conn_ptr->read_all(data.c_str(), data.length());
    }

    /*template<typename config_type>
    void wrapper<config_type>::perform_send(std::shared_ptr<ssl::connection> conn_ptr) {
        std::string data;

        for (int i = 0; i < m_send_bufs.size(); i++) {
            data.append(m_send_bufs[i].buf, m_send_bufs[i].len);
        }

        conn_ptr->send(data);
    }*/

    template<typename config_type>
    void wrapper<config_type>::close() {
        if (m_tmp_conn_ptr == nullptr) {
            throw hc::exception("no connection being used", "hc::net::ws::wrapper::recv");
        }

        m_ws_conn_ptr->close(websocketpp::close::status::going_away, "connection closed by server");
        
        m_closed = true;
    }

    template<typename config_type>
    std::error_code wrapper<config_type>::on_vector_write(websocketpp::connection_hdl hdl, std::vector<websocketpp::transport::buffer> const& bufs) {
        if (m_tmp_conn_ptr == nullptr) {
            throw hc::exception("no connection being used", "hc::net::ws::wrapper::recv");
        }
        
        std::string data;

        for (int i = 0; i < bufs.size(); i++) {
            data.append(bufs[i].buf, bufs[i].len);
        }

        m_tmp_conn_ptr->send(data);

        return std::error_code();
    }

    template<typename config_type>
    std::error_code wrapper<config_type>::on_write(websocketpp::connection_hdl hdl, const char* data, std::size_t len) {
        if (m_tmp_conn_ptr == nullptr) {
            throw hc::exception("no connection being used", "hc::net::ws::wrapper::recv");
        }
        
        m_tmp_conn_ptr->send(std::string(data, len));

        return std::error_code();
    }

    template<typename config_type>
    void wrapper<config_type>::on_open(websocketpp::connection_hdl ws_conn_hdl) {
        util::logger::dbg("websocket connected");
        m_closed = false;
    }

    template<typename config_type>
    void wrapper<config_type>::on_fail(websocketpp::connection_hdl ws_conn_hdl) {
        util::logger::dbg("websocket handshake failed");
        m_closed = true;
    }

    template<typename config_type>
    void wrapper<config_type>::on_close(websocketpp::connection_hdl ws_conn_hdl) {
        util::logger::dbg("websocket disconnected");
        m_closed = true;
    }

    template<typename config_type>
    void wrapper<config_type>::on_message(websocketpp::connection_hdl ws_conn_hdl, std::shared_ptr<websocketpp::config::core::message_type> msg_ptr) {
        m_message_log.push_back(msg_ptr->get_payload());
    }

    template class wrapper<server_config>;
    template class wrapper<client_config>;

}
}
}