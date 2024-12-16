#pragma once
#include "codec.hpp"
#include "limit_order_engine.h"
#include "server_session.h"
#include <asio.hpp>
#include <asio/error_code.hpp>
#include <asio/ip/tcp.hpp>
#include <iostream>

using asio::ip::tcp;
using asio::error_code;

namespace limit_order {


class server : public std::enable_shared_from_this<server>
{
    limit_order::limit_order_engine* _eng;
    server_codec & _server_codec;
public:
    server(asio::io_context& io_ctx, short port, limit_order::limit_order_engine* eng, limit_order::server_codec& server_codec)
            : _eng(eng),
              _server_codec(server_codec),
              io_context(io_ctx),
              m_acceptor(io_ctx, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }

    void do_accept() {
        m_acceptor.async_accept([this](error_code ec, tcp::socket socket) {
            if (!ec) {
                auto session =  std::make_shared<server_session>(std::move(socket), io_context, _server_codec);
                session->set_engine(_eng);
                session->run();

            } else {
                std::cout << "error: " << ec.message() << std::endl;
            }
            do_accept();
        });
    }
private:
    asio::io_context& io_context;
    tcp::acceptor m_acceptor;
};

}