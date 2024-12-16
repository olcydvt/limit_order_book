#pragma once
#include "limit_order_book.hpp"
#include "messages.hpp"
#include <expected>
#include <optional>
#include <asio.hpp>
#include "codec.hpp"
#include <asio/ip/tcp.hpp>
using asio::ip::tcp;


namespace limit_order {

    class limit_order_engine {
        limit_order_book& _limit_order_book;
        server_codec& _server_codec;
        client_codec& _client_codec;
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

        explicit limit_order_engine(limit_order_book& order_book, server_codec& server_codec,  client_codec& client_codec);
        void add_buy_order(int price, int amount);
        void add_sell_order(int price, int amount);
        limit_order_book::executed_order process_order(const order& _order, const std::string_view symbol);
        void remove_order(const order& _order, const std::string_view symbol);
        void on_request(limit_order::place_order_messasge& message,  tcp::socket& m_socket);
        void on_request(limit_order::cancel_order_message& message, tcp::socket& m_socket);

    private:
        std::expected<order_placed, request_rejected> confirm_order(const order& _order, const std::string_view symbol);
    };

}
