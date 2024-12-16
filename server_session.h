#pragma once
#include <iostream>
#include <asio.hpp>
#include <asio/error_code.hpp>
#include <asio/ip/tcp.hpp>
#include "codec.hpp"
#include "limit_order_engine.hpp"

using asio::ip::tcp;
using asio::error_code;

namespace limit_order {

class server_session
    : public std::enable_shared_from_this<server_session>
    , public server_codec {
    tcp::socket m_socket;
    asio::streambuf m_buffer;
    asio::io_context& io;
    using clock_type = std::chrono::system_clock;
    using executor_type = asio::io_context::executor_type;
    limit_order_engine* _engine;

public:
    server_session(tcp::socket socket, asio::io_context& io)
    : m_socket(std::move(socket))
    , io(io) {
    }

    void set_engine(limit_order_engine* engine) {
        _engine = engine;
    }

    void run() {
        wait_for_request();
    }

    void handler(std::string& data)
    {
        if (data.size() < 5) { // 5 bytes (1 message type + 4 message size)
            std::cout << "invalid message" << "\n";
            return;
        }
        uint8_t message_type =  this->get_message_type(data);
        switch (message_type) {
            case order_placed_message::message_type_value: {
                limit_order::place_order_messasge message;
                this->parse_message(message, data);
                _engine->on_request(message, this);
                break;
            }
            case  place_order_messasge::message_type_value: {
                limit_order::place_order_messasge message;
                this-> parse_message(message, data);
                _engine->on_request(message, this);
                break;
            }
        }

    }

    void on_response(order_placed_message& msg) {
        std::string data = this->serialize_message(msg);
        asio::write(m_socket, asio::buffer(data));
    }

    void on_response(order_traded& msg) {
        std::string data = this->serialize_message(msg);
        asio::write(m_socket, asio::buffer(data));
    }

private:
    void wait_for_request() {
        auto self(this->shared_from_this());
        asio::async_read_until(m_socket, m_buffer, "\0",
        [this, self](error_code ec, std::size_t /*length*/)
        {
            if (!ec)  {
                std::string data{
                    std::istreambuf_iterator<char>(&m_buffer),
                    std::istreambuf_iterator<char>()
                };
                handler(data);
                wait_for_request();
            } else {
                m_socket.shutdown(asio::socket_base::shutdown_both, ec);
                // TODO : Insert log message here
                std::cout << "socket for: "<< m_socket.remote_endpoint().address().to_string() <<
                    " " << m_socket.remote_endpoint().port() << " has been closed because of: " <<  ec << std::endl;
            }
        });
    }
};
};