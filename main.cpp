#include "codec.hpp"
#include "limit_order_book.hpp"
#include "limit_order_server.h"
#include "server_session.h"
#include <latch>
#include <thread>
using namespace std;

void send_trade_from_x() {
    using asio::ip::tcp;
    asio::io_context io_context;
    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);
    asio::connect(socket, resolver.resolve("127.0.0.1", "25000"));

    cout << "Sending trade" << endl;
    limit_order::place_order_messasge message;
    message.order_id = 123;
    message.amount = 5;
    message.price = 100;
    message.symbol = "EURUSD";
    limit_order::client_codec client_codec;
    auto msg = client_codec.serialize_message(message);
    auto result = asio::write(socket, asio::buffer(msg));
}

void send_trade_from_y() {
    using asio::ip::tcp;
    asio::io_context io_context;
    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);
    asio::connect(socket, resolver.resolve("127.0.0.1", "25000"));

    cout << "Sending trade" << endl;
    limit_order::cancel_order_message message;
    message.order_id = 123;
    limit_order::client_codec client_codec;
    auto msg = client_codec.serialize_message(message);
    auto result = asio::write(socket, asio::buffer(msg));
}

int main()
{

    limit_order::limit_order_book order_book;
    limit_order::client_codec client_codec;
    limit_order::server_codec server_codec;
    limit_order::limit_order_engine eng(order_book, server_codec, client_codec);
    std::latch latch{1};
    eng.add_buy_order(100, 10);
    eng.add_buy_order(99, 5);
    eng.add_sell_order(101, 8);
    eng.add_sell_order(102, 15);
    std::jthread client_1(send_trade_from_x);
    std::jthread client_2(send_trade_from_y);
    asio::io_context io_context;
    limit_order::server limit_order_server{io_context, 25000, &eng, server_codec};
    io_context.run();

    latch.wait();

    return 0;

}