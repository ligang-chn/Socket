cmake_minimum_required(VERSION 3.15)
project(Socket)

set(CMAKE_CXX_STANDARD 17)
link_libraries(ws2_32 wsock32)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")


add_executable(Socket main.cpp DataStruct.h)
add_executable(server server/server.cpp server/SocketServer.cpp server/SocketServer.h)
add_executable(client client/client.cpp client/SocketClient.cpp client/SocketClient.h)
