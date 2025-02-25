#pragma once

#include <set>
#include <unordered_map>
#include <map>
/*

buy/long/+
sell/short/-
if the order is but then price will be (+) integer
controversially if the order is sell then price will be (-) integer
*/

class order_book {
    using order_volume = int;
    using order_price = int;
    using order_id = int;
    using order_book_entry_iterator = std::map<order_price, std::pair<order_volume, order_id>>::iterator;
    using order_book_entry = order_book_entry_iterator::reference;

    std::map<order_price, std::pair<order_volume, order_id>> sell_order_book_map;
    std::map<order_price, std::pair<order_volume, order_id>, std::greater<int>> buy_order_book_map;
    std::unordered_map<order_id, order_book_entry_iterator> order_id_to_book_map;

    inline void check_and_execute_trade(order_id id, order_price requested_price) {
        order_book_entry_iterator order_book_entry_itr;
        if (requested_price > 0) {
            order_book_entry_itr = sell_order_book_map.begin();
            if (order_book_entry_itr == sell_order_book_map.end()) {
                return; // no sell trade keep waiting available sell limit
            }
            const order_price& booked_order_price = order_book_entry_itr->first;
            order_volume& booked_order_volume = order_book_entry_itr->second.first;
            order_id booked_order_id = order_book_entry_itr->second.second;

            if (requested_price >= booked_order_price) {
                order_volume& requested_volume = buy_order_book_map[requested_price].first;
                requested_volume -= booked_order_volume;
                if (requested_volume > 0) { //partially fill & booked order also full filled
                    order_id_to_book_map.erase(booked_order_id);
                    sell_order_book_map.erase(booked_order_price);
                    check_and_execute_trade(id, requested_price);
                }
                else if (requested_volume < 0) { //full fill & booked order partially filled
                    booked_order_volume = -requested_volume;
                    order_id_to_book_map.erase(id);
                    buy_order_book_map.erase(requested_price);
                }
                else { //full fill & booked order also full filled
                    order_id_to_book_map.erase(booked_order_id);
                    sell_order_book_map.erase(booked_order_price);

                    order_id_to_book_map.erase(id);
                    buy_order_book_map.erase(requested_price);
                }

            }
        }
        else {
            order_book_entry_itr = buy_order_book_map.begin();
            if (order_book_entry_itr == buy_order_book_map.end()) {
                return; // no sell trade keep waiting available buy limit
            }
            const order_price& booked_order_price = order_book_entry_itr->first;
            order_volume& booked_order_volume = order_book_entry_itr->second.first;
            order_id booked_order_id = order_book_entry_itr->second.second;
            order_price normilezed_price = -requested_price;
            if (normilezed_price <= booked_order_price) {
                order_volume& requested_volume = sell_order_book_map[normilezed_price].first;
                requested_volume -= booked_order_volume;
                if (requested_volume > 0) { //partially fill & booked order also full filled
                    order_id_to_book_map.erase(booked_order_id);
                    buy_order_book_map.erase(booked_order_price);
                    check_and_execute_trade(id, requested_price);
                }
                else if (requested_volume < 0) { //full fill & booked order partially filled
                    booked_order_volume = -requested_volume;
                    order_id_to_book_map.erase(id);
                    sell_order_book_map.erase(normilezed_price);
                }
                else { //full fill & booked order also full filled
                    order_id_to_book_map.erase(booked_order_id);
                    sell_order_book_map.erase(booked_order_price);

                    order_id_to_book_map.erase(id);
                    buy_order_book_map.erase(normilezed_price);
                }
            }
        }
    }

public:

    void place_order(order_id id, order_price price, order_volume volume ) {
        if (price < 0) {
            int sell_price = -price;
            sell_order_book_map[sell_price].first += volume;
            sell_order_book_map[sell_price].second = id;
            order_id_to_book_map[id] = sell_order_book_map.find(sell_price);
        }
        else {
            buy_order_book_map[price].first += volume;
            buy_order_book_map[price].second = id;
            order_id_to_book_map[id] = buy_order_book_map.find(price);
        }

        check_and_execute_trade(id, price);
    }

};


