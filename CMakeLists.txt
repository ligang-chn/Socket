cmake_minimum_required(VERSION 3.15)
project(Socket)

set(CMAKE_CXX_STANDARD 17)
link_libraries(ws2_32 wsock32)

add_executable(Socket main.cpp )
add_executable(server server/server.cpp)
add_executable(client client/client.cpp)
