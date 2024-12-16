#include "server_session.h"
#include "codec.hpp"
#include <asio.hpp>
#include <asio/error_code.hpp>
#include <asio/ip/tcp.hpp>
#include <iostream>

using asio::error_code;
using asio::ip::tcp;

using namespace limit_order;

server_session::server_session(tcp::socket socket, asio::io_context& io, server_codec& codec)
        : m_socket(std::move(socket)),
          io(io),
        _codec(codec){
}

void server_session::set_engine(limit_order_engine* engine) {
    _engine = engine;
}

void server_session::run() {
    wait_for_request();
}

void server_session::handler(std::string& data) {
    if (data.size() < 5) { // 5 bytes (1 message type + 4 message size)
        std::cout << "invalid message"
                  << "\n";
        return;
    }
    uint8_t message_type = _codec.get_message_type(data);
    switch (message_type) {
        case order_traded::message_type_value: {
            limit_order::place_order_messasge message;
            _codec.parse_message(message, data);
            _engine->on_request(message, m_socket);
            break;
        }
        case place_order_messasge::message_type_value: {
            limit_order::place_order_messasge message;
            _codec.parse_message(message, data);
            _engine->on_request(message, m_socket);
            break;
        }
    }
}

void server_session::on_response(order_placed_message& msg) {
    std::string data =_codec.serialize_message(msg);
    asio::write(m_socket, asio::buffer(data));
}

void server_session::on_response(order_traded& msg) {
    std::string data = _codec.serialize_message(msg);
    asio::write(m_socket, asio::buffer(data));
}

void server_session::wait_for_request() {
    auto self(this->shared_from_this());
    asio::async_read_until(m_socket, m_buffer, "\0",
                           [this, self](error_code ec, std::size_t /*length*/) {
                               if (!ec) {
                                   std::string data{
                                       std::istreambuf_iterator<char>(&m_buffer),
                                       std::istreambuf_iterator<char>()};
                                   handler(data);
                                   wait_for_request();
                               } else {
                                   m_socket.shutdown(asio::socket_base::shutdown_both, ec);
                                   std::cout << "socket for: " << m_socket.remote_endpoint().address().to_string() << " " << m_socket.remote_endpoint().port() << " has been closed because of: " << ec << std::endl;
                               }
                           });
}
