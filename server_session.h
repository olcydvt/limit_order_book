#pragma once
#include "codec.hpp"
#include "limit_order_engine.h"
#include <asio.hpp>
#include <asio/error_code.hpp>
#include <asio/ip/tcp.hpp>

using asio::error_code;
using asio::ip::tcp;

namespace limit_order {

    class server_session
            : public std::enable_shared_from_this<server_session> {
        tcp::socket m_socket;
        asio::streambuf m_buffer;
        asio::io_context& io;
        using clock_type = std::chrono::system_clock;
        using executor_type = asio::io_context::executor_type;
        limit_order_engine* _engine;
        server_codec& _codec;

    public:
        server_session(tcp::socket socket, asio::io_context& io, server_codec& _codec);
        void set_engine(limit_order_engine* engine);
        void run();
        void handler(std::string& data);

        void on_response(order_placed_message& msg);
        void on_response(order_traded& msg);

    private:
        void wait_for_request();
    };
};
