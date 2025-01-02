#pragma once
#include <iostream>
#include <map>
#include <mutex>

namespace limit_order {

    class limit_order_book {
    private:
        using price = int;
        using amount = int;
        using order_id = int;
        std::map<price, amount, std::greater<>> buy_orders;
        std::map<price, amount> sell_orders;

        // TODO: can be lock free
        std::mutex buy_orders_mutex;
        std::mutex sell_orders_mutex;

    public:
        enum class order_side : int {
            buy = 0,
            sell = 1
        };

        struct executed_order {
            int traded_price;
            int traded_amount;
        };

        // Add a buy order (price, amount)
        // Accumulate amount at the same price
        void add_buy_order(int price, int amount) {
            std::lock_guard<std::mutex> lock(buy_orders_mutex);
            buy_orders[price] += amount;
        }

        // Add a sell order (price, amount)
        // Accumulate amount at the same price
        void add_sell_order(int price, int amount) {
            std::lock_guard<std::mutex> lock(sell_orders_mutex);
            sell_orders[price] += amount;
        }

        void print_order_book() {
            std::lock_guard<std::mutex> lock_sell(sell_orders_mutex);
            std::cout << "Sell Orders:\n";
            for (const auto& [price, amount]: sell_orders) {
                std::cout << "  Price: " << price << ", Amount: " << amount << '\n';
            }
            std::lock_guard<std::mutex> lock_buy(buy_orders_mutex);
            std::cout << "Buy Orders:\n";
            for (const auto& [price, amount]: buy_orders) {
                std::cout << "  Price: " << price << ", Amount: " << amount << '\n';
            }
        }

        executed_order process_order(order_side _order_side, int price, int amount) {
            int initial_amount  = amount;
            if (_order_side == order_side::buy) {
                std::lock_guard<std::mutex> lock_sell(sell_orders_mutex);

                while (!sell_orders.empty() && price >= sell_orders.begin()->first && amount > 0) {
                    auto [sell_price, sell_amount] = *sell_orders.begin();
                    if (amount >= sell_amount) {
                        amount -= sell_amount;
                        sell_orders.erase(sell_orders.begin());
                    } else {
                        sell_orders[sell_price] -= amount;
                        amount = 0;
                    }
                }
                if (amount > 0)
                    add_buy_order(price, amount); // Add remaining amount to buy orders
            } else {
                std::lock_guard<std::mutex> lock_buy(buy_orders_mutex);
                while (!buy_orders.empty() && price <= buy_orders.begin()->first && amount > 0) {
                    auto [buy_price, buy_amount] = *buy_orders.begin();
                    if (amount >= buy_amount) {
                        amount -= buy_amount;
                        buy_orders.erase(buy_orders.begin());
                    } else {
                        buy_orders[buy_price] -= amount;
                        amount = 0;
                    }
                }
                if (amount > 0)
                    add_sell_order(price, amount); // Add remaining amount to sell orders
            }

            return executed_order{price, initial_amount - amount};
        }

        void cancel_order (order_id _id) {
            // TODO: query order, if exist remove from the book
        }
    };
}
