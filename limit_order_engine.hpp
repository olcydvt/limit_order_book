#pragma once
#include "limit_order_book.hpp"
#include "messages.hpp"
#include <optional>
#include <expected>
#include "server_session.h"

namespace limit_order {

    class limit_order_engine {
        limit_order_book& _limit_order_book;
        inline static constexpr const char* _symbol = "EURUSD";

    public:
        struct order {
            limit_order_book::order_side order_side;
            int order_id;
            int price;
            int amount;
        };

        struct order_placed {
            int order_id;
            int price;
            int amount;
        };

        struct order_cancelled {
            int order_id;
            std::optional<int> reason_code; // Optional
        };

        struct request_rejected {
            int order_id;
            std::string_view reason = "invalid symbol";
        };

        explicit limit_order_engine(limit_order_book& order_book)
                : _limit_order_book(order_book) {}

        void add_buy_order(int price, int amount) {
            _limit_order_book.add_buy_order(price, amount);
        }

        void add_sell_order(int price, int amount) {
            _limit_order_book.add_sell_order(price, amount);
        }

         limit_order_book::executed_order process_order(const order& _order, const std::string_view symbol) {
            return _limit_order_book.process_order(_order.order_side, _order.price, _order.amount);
        }

        void remove_order(const order& _order, const std::string_view symbol) {
            _limit_order_book.cancel_order(_order.order_id);
        }

        void on_request(limit_order::place_order_messasge& message, server_session* session) {
            order order_req;
            order_req.order_id = message.order_id;
            order_req.price = message.price;
            order_req.amount = message.amount;
            order_req.order_side =  static_cast<limit_order_book::order_side>(message.order_side);

            if (const auto order_placed = confirm_order(order_req, message.symbol); order_placed.has_value()) {
                order_placed_message confirmed_order;
                confirmed_order.price = order_req.price;
                confirmed_order.order_id = order_req.order_id;
                confirmed_order.amount = order_req.amount;
                session->on_response(confirmed_order);
                limit_order_book::executed_order executed_order = process_order(order_req, message.symbol);
                order_traded _order_traded;
                _order_traded.order_id = order_req.order_id;
                _order_traded.amount = executed_order.traded_amount;
                _order_traded.price = executed_order.traded_price;
                session->on_response(_order_traded);
            } else {
                std::cout << "TODO: apply reject message" << std::endl;
                //TODO: apply reject message
            }
        }

        void on_request(limit_order::cancel_order_message& message, server_session* session) {
            // TODO: notify client with order confirmed message
        }

    private:
        std::expected<order_placed, request_rejected> confirm_order(const order& _order, const std::string_view symbol) {
            if (symbol.compare(_symbol)) {
                return std::unexpected(request_rejected{ _order.order_id, symbol});
            } else {
                return order_placed{_order.order_id, _order.price, _order.amount};
            }
        }
    };

}
