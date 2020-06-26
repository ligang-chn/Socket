# cmake 最低版本需求
cmake_minimum_required(VERSION 3.5)

# 工程名称
project(socket)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 添加 math 子目录
#add_subdirectory(math)

# 设置
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread") #添加线程选项

# 编译源码生成目标
add_executable(client src/client.cpp)
add_executable(server src/server.cpp)

# 添加链接库
#target_link_libraries(Demo MathFunctions)