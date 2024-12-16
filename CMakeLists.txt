cmake_minimum_required (VERSION 3.8)
project (limit_order_engine)

set(CMAKE_CXX_STANDARD 23)

find_package(asio REQUIRED)
find_path(PROTOZERO_INCLUDE_DIRS "protozero/basic_pbf_builder.hpp")

add_executable (limit_order_engine  "main.cpp")

target_link_libraries(limit_order_engine PUBLIC
        asio::asio
)