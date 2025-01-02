#pragma once
#include <string>
#include <protozero/basic_pbf_writer.hpp>
#include <protozero/pbf_writer.hpp>
#include <protozero/pbf_reader.hpp>
#include "messages.hpp"

namespace limit_order {
    struct server_codec {
        uint8_t get_message_type(std::string &data) {
            auto *_data = (uint8_t *) (&data[0]);
            return _data[0];
        }

        void parse_message(limit_order::place_order_messasge &message, std::string &data) {
            auto *_data = (uint8_t *) (&data[0]);
            uint32_t message_size;
            std::memcpy(&message_size, _data + 1, sizeof(int32_t));

            protozero::pbf_reader reader{reinterpret_cast<const char *>(_data + 5), message_size};
            while (reader.next()) {
                switch (reader.tag()) {
                    case limit_order::place_order_messasge::order_id_tag:
                        message.order_id = reader.get_int32();
                        break;
                    case limit_order::place_order_messasge::order_side_tag:
                        message.order_side = static_cast<limit_order::order_side>(reader.get_int32());
                        break;
                    case limit_order::place_order_messasge::price_tag:
                        message.price = reader.get_int32();
                        break;
                    case limit_order::place_order_messasge::amount_tag:
                        message.amount = reader.get_int32();
                        break;
                    case limit_order::place_order_messasge::symbol_tag:
                        message.symbol = reader.get_string();
                        break;
                    default:
                        reader.skip();
                }
            }
        }

        void parse_message(limit_order::cancel_order_message &message, std::string &data) {
            auto *_data = (uint8_t *) (&data[0]);
            uint32_t message_size;
            std::memcpy(&message_size, _data + 1, sizeof(int32_t));

            protozero::pbf_reader reader{reinterpret_cast<const char *>(_data + 5), message_size};
            while (reader.next()) {
                switch (reader.tag()) {
                    case limit_order::cancel_order_message::order_id_tag:
                        message.order_id = reader.get_int32();
                    break;
                    default:
                        reader.skip();
                }
            }
        }

        std::string serialize_message(const order_placed_message &msg) noexcept {
            std::string data;
            std::string header;
            header.append(reinterpret_cast<const char *>(&order_placed_message::message_type_value), 1);
            protozero::pbf_writer writer{data};
            writer.add_int32(order_placed_message::order_id_tag, msg.order_id);
            writer.add_int32(order_placed_message::amount_tag, msg.amount);
            writer.add_int32(order_placed_message::price_tag, msg.price);

            header.resize(header.size() + 4);
            size_t length = data.length();
            std::memcpy((uint8_t *) &header[1], &length, sizeof(uint32_t));
            data.insert(0, header);
            return data;
        }

        std::string serialize_message(const order_cancelled &msg) noexcept {
            std::string data;
            std::string header;
            header.append(reinterpret_cast<const char *>(&order_placed_message::message_type_value), 1);
            protozero::pbf_writer writer{data};
            writer.add_int32(order_placed_message::order_id_tag, msg.order_id);
            header.resize(header.size() + 4);
            size_t length = data.length();
            std::memcpy((uint8_t *) &header[1], &length, sizeof(uint32_t));
            data.insert(0, header);
            return data;
        }

        std::string serialize_message(const order_traded &msg) noexcept {
            std::string data;
            std::string header;
            header.append(reinterpret_cast<const char *>(&order_traded::message_type_value), 1);
            protozero::pbf_writer writer{data};
            writer.add_int32(order_traded::order_id_tag, msg.order_id);
            writer.add_int32(order_traded::amount_tag, msg.amount);
            writer.add_int32(order_traded::price_tag, msg.price);

            header.resize(header.size() + 4);
            size_t length = data.length();
            std::memcpy((uint8_t *) &header[1], &length, sizeof(uint32_t));
            data.insert(0, header);
            return data;
        }
    };

    struct client_codec {
        uint8_t get_message_type(std::string &data) {
            auto *_data = (uint8_t *) (&data[0]);
            return _data[0];
        }

        std::string serialize_message(const place_order_messasge &msg) noexcept {
            std::string data;
            std::string header;
            header.append(reinterpret_cast<const char *>(&place_order_messasge::message_type_value), 1);
            protozero::pbf_writer writer{data};
            writer.add_int32(place_order_messasge::order_id_tag, msg.order_id);
            writer.add_int32(place_order_messasge::amount_tag, msg.amount);
            writer.add_int32(place_order_messasge::price_tag, msg.price);
            writer.add_string(place_order_messasge::symbol_tag, msg.symbol);

            header.resize(header.size() + 4);
            size_t length = data.length();
            std::memcpy((uint8_t *) &header[1], &length, sizeof(uint32_t));
            data.insert(0, header);
            return data;
        }

        void parse_message(limit_order::order_reject_message &message) {
            //TODO : to be implemented
        }

        void parse_message(limit_order::order_placed_message &message, std::string &data) {
            const auto *_data = reinterpret_cast<uint8_t *>(&data[0]);
            uint32_t message_size;
            std::memcpy(&message_size, _data + 1, sizeof(int32_t));

            protozero::pbf_reader reader{reinterpret_cast<const char *>(_data + 5), message_size};
            while (reader.next()) {
                switch (reader.tag()) {
                    case limit_order::order_placed_message::order_id_tag:
                        message.order_id = reader.get_int32();
                        break;
                    case limit_order::order_placed_message::price_tag:
                        message.price = reader.get_int32();
                        break;
                    case limit_order::order_placed_message::amount_tag:
                        message.amount = reader.get_int32();
                        break;
                    default:
                        reader.skip();
                }
            }
        }

        void parse_message(limit_order::order_traded &message, std::string &data) {
            const auto *_data = reinterpret_cast<uint8_t *>(&data[0]);
            uint32_t message_size;
            std::memcpy(&message_size, _data + 1, sizeof(int32_t));

            protozero::pbf_reader reader{reinterpret_cast<const char *>(_data + 5), message_size};
            while (reader.next()) {
                switch (reader.tag()) {
                    case limit_order::order_traded::order_id_tag:
                        message.order_id = reader.get_int32();
                        break;
                    case limit_order::order_traded::price_tag:
                        message.price = reader.get_int32();
                        break;
                    case limit_order::order_traded::amount_tag:
                        message.amount = reader.get_int32();
                        break;
                    default:
                        reader.skip();
                }
            }
        }

        void parse_message(limit_order::order_cancelled &message, std::string &data) {
            const auto *_data = reinterpret_cast<uint8_t *>(&data[0]);
            uint32_t message_size;
            std::memcpy(&message_size, _data + 1, sizeof(int32_t));

            protozero::pbf_reader reader{reinterpret_cast<const char *>(_data + 5), message_size};
            while (reader.next()) {
                switch (reader.tag()) {
                    case limit_order::order_cancelled::order_id_tag:
                        message.order_id = reader.get_int32();
                    break;
                    default:
                        reader.skip();
                }
            }
        }

        std::string serialize_message(limit_order::cancel_order_message &message) {
            std::string data;
            std::string header;
            header.append(reinterpret_cast<const char *>(&cancel_order_message::message_type_value), 1);
            protozero::pbf_writer writer{data};
            writer.add_int32(cancel_order_message::order_id_tag, message.order_id);

            header.resize(header.size() + 4);
            size_t length = data.length();
            std::memcpy((uint8_t *) &header[1], &length, sizeof(uint32_t));
            data.insert(0, header);
            return data;
        }
    };
}
