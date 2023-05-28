cmake_minimum_required(VERSION 3.18)

project(oil_storage_management_system)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

add_link_options(-pthread -Wall)
add_executable(server server.cpp)
add_executable(client client.cpp)
