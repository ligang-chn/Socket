###### 1 文档结构说明
文件夹source中包含的是一份完整的源码，适合跨平台。可以直接拷贝该文件夹下的文件使用。
使用CMake直接编译。

除此之外的文件都是CLion环境下编译的文件。
###### 2 环境
任选其一：

1）编译工具：CLion
（需支持C++ 11）

2）编译工具：G++
(使用cmake编译)

###### 3 使用说明
建议直接拷贝source文件夹，然后在该文件夹下使用cmake编译。

###### 4 提交日志
1) 简易C/S通信
2) 建立能持续处理请求的C/S网络程序
3) 发送结构化的网络消息数据
4) 网络报文定义与发送
5) 将多次收发报文数据升级为一次收发
6) 网络消息接收长度
7) 服务端升级为select模型处理多客户端
8) 客户端升级为select模型
9) 跨平台统一代码，linux+windows
10) 封装Client类，同时与多个服务端通信
11) 封装Server类，同时开启多个服务器（tag）
12) TCP粘包测试
13) 单线程select模型10000连接测试
14） 服务端单线程模式下性能瓶颈测试
    （性能消耗主要在select查询中数据可不可以读等操作。
        在初期进行客户端连接时，消耗不是很大，但是一旦进行收数据时，
        同时进行客户端连接，就会连接的很慢。
     目标：a、分离客户端连接的函数
          b、分摊单线程下查询数据有没有数据可以读等操作）