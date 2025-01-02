#include "limit_order_engine.h"
#include <print>

using namespace limit_order;

limit_order_engine::limit_order_engine(limit_order_book &order_book, server_codec &server_codec,
                                       client_codec &client_codec)
    : _limit_order_book(order_book),
      _server_codec(server_codec),
      _client_codec(client_codec) {
}

void limit_order_engine::add_buy_order(int price, int amount) {
    _limit_order_book.add_buy_order(price, amount);
}

void limit_order_engine::add_sell_order(int price, int amount) {
    _limit_order_book.add_sell_order(price, amount);
}

limit_order_book::executed_order limit_order_engine::process_order(const order &_order, const std::string_view symbol) {
    return _limit_order_book.process_order(_order.order_side, _order.price, _order.amount);
}

void limit_order_engine::remove_order(int order_id) {
    _limit_order_book.cancel_order(order_id);
}

void limit_order_engine::on_request(limit_order::place_order_messasge &message, tcp::socket &_socket) {
    order order_req;
    order_req.order_id = message.order_id;
    order_req.price = message.price;
    order_req.amount = message.amount;
    order_req.order_side = static_cast<limit_order_book::order_side>(message.order_side);

    if (const auto order_placed = confirm_order(order_req, message.symbol); order_placed.has_value()) {
        order_placed_message confirmed_order;
        confirmed_order.price = order_req.price;
        confirmed_order.order_id = order_req.order_id;
        confirmed_order.amount = order_req.amount;
        std::string confirmed_data = _server_codec.serialize_message(confirmed_order);
        auto result = asio::write(_socket, asio::buffer(confirmed_data, sizeof(order_placed_message)));
        std::println( "confirmed_order data sent for: {}", confirmed_order.order_id);

        limit_order_book::executed_order executed_order = process_order(order_req, message.symbol);

        order_traded _order_traded;
        _order_traded.order_id = order_req.order_id;
        _order_traded.amount = executed_order.traded_amount;
        _order_traded.price = executed_order.traded_price;
        std::string order_traded_data = _server_codec.serialize_message(_order_traded);
        result = asio::write(_socket, asio::buffer(order_traded_data, sizeof(order_traded)));
        std::println("order fullfilled data sent for: {} ", _order_traded.order_id );

    } else {
        std::cout << "TODO: apply reject message" << std::endl;
        //TODO: apply reject message
    }
}

void limit_order_engine::on_request(limit_order::cancel_order_message &message, tcp::socket &m_socket) {
    remove_order( message.order_id);
}

std::expected<limit_order_engine::order_placed, limit_order_engine::request_rejected> limit_order_engine::confirm_order(
    const order &_order, const std::string_view symbol) {
    if (symbol.compare(_symbol)) {
        return std::unexpected(request_rejected{_order.order_id, symbol});
    } else {
        return order_placed{_order.order_id, _order.price, _order.amount};
    }
}
