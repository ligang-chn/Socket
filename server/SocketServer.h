//
// Created by ligang on 2020/7/22.
//

#ifndef SOCKET_SOCKETSERVER_H
#define SOCKET_SOCKETSERVER_H

#define  WIN32_LEAN_AND_MEAN //主要解决WinSock2.h头文件引入问题

#ifdef _WIN32
    #include <windows.h>
    #include <winsock2.h>
    #include <inaddr.h>//这个可能是之前clion自动引入的

    #include "../DataStruct.h"
#else
    #include <unistd.h>//uni std
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <string.h>
    #include <stdlib.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include "./DataStruct.h"

    #define SOCKET int
    #define INVALID_SOCKET (SOCKET)(~0)
    #define SOCKET_ERROR           (-1)
#endif

#include <iostream>
#include <stdlib.h>
#include <thread>
#include <vector>
#include <algorithm>

#define  RECV_BUFF_SIZE 100     //缓冲区最小单元大小

class SocketServer {
private:
    SOCKET _sock;
    std::vector<SOCKET> g_clients;
    char _szRecv[RECV_BUFF_SIZE]={}; //接收缓冲区
//    char _szMsgBuf[RECV_BUFF_SIZE*10]={}; //第二/消息缓冲区
//    int _lastPos=0;
    int _nCount=0;

public:
    SocketServer();
    virtual ~SocketServer();

    //初始化socket
    SOCKET InitSocket();

    //绑定端口号
    int Bind(const char* ip, unsigned short port);

    //监听端口
    int Listen(int n);

    //接受客户端连接
    SOCKET Accept();
    //关闭socket
    void Close();

    //处理网络消息
    bool OnRun();

    //是否工作中
    bool isRun();

    //接收数据 处理粘包 拆分包
    int RecvData(SOCKET _cSock);

    //响应网络消息
    virtual void OnNetMsg(SOCKET _cSock, DataHeader* header);

    //发送指定Socket数据
    int SendData(SOCKET _cSock, DataHeader* header);

    //群发消息
    void SendDataToAll(DataHeader* header);
};


#endif //SOCKET_SOCKETSERVER_H
