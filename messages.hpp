#pragma once
#include <string>

namespace limit_order {
    enum class order_side : int {
        buy = 0,
        sell = 1
    };
    struct place_order_messasge {
        static inline constexpr const uint8_t message_type_value = 1;
        static inline constexpr const uint32_t order_id_tag = 1;
        static inline constexpr const uint32_t order_side_tag = 2;
        static inline constexpr const uint32_t price_tag = 3;
        static inline constexpr const uint32_t amount_tag = 4;
        static inline constexpr const uint32_t symbol_tag = 5;

        int order_id;
        int price;
        int amount;
        order_side order_side;
        std::string symbol;
    };

    struct order_placed_message {
        static inline constexpr const uint8_t message_type_value = 2;
        static inline constexpr const uint32_t order_id_tag = 1;
        static inline constexpr const uint32_t price_tag = 2;
        static inline constexpr const uint32_t amount_tag = 3;

        int order_id;
        int price;
        int amount;
    };

    struct order_traded {
        static inline constexpr const uint8_t message_type_value = 3;
        static inline constexpr const uint32_t order_id_tag = 1;
        static inline constexpr const uint32_t price_tag = 2;
        static inline constexpr const uint32_t amount_tag = 3;

        int order_id;
        int price;
        int amount;
    };

    struct cancel_order_message {
        static inline constexpr const uint8_t message_type_value = 4;
        static inline constexpr const uint32_t order_id_tag = 1;
        int order_id;

    };

    struct order_reject_message {
        static inline constexpr const uint8_t message_type_value = 5;
        // TODO: requires implementation

    };

   }