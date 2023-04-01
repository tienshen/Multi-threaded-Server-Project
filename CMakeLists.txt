cmake_minimum_required(VERSION 3.24)
project(ECE670_mutli_thread_server_project)

set(CMAKE_CXX_STANDARD 17)

add_executable(ECE670_mutli_thread_server_project main.cpp server.cpp server.h cmake-build-debug/client.cpp cmake-build-debug/client.h sample_socket/socket_server.cpp sample_socket/socket_client.cpp)
