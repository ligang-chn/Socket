//
// Created by ligang on 2020/6/26.
//

#ifndef SOCKET_SOCKETCLIENT_H
#define SOCKET_SOCKETCLIENT_H

#define  WIN32_LEAN_AND_MEAN //主要解决WinSock2.h头文件引入问题

#ifdef _WIN32
    #define FD_SETSIZE 2506
    #include <windows.h>
    #include <WinSock2.h>
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

#ifndef  RECV_BUFF_SIZE
#define  RECV_BUFF_SIZE 10240     //缓冲区最小单元大小
#endif

class SocketClient {
private:
    SOCKET _sock;
    char _szRecv[RECV_BUFF_SIZE]={}; //接收缓冲区
    char _szMsgBuf[RECV_BUFF_SIZE*5]={}; //第二/消息缓冲区
    int _lastPos=0;  //消息缓冲区的数据尾部位置
    int _nCount=0;
    bool _isConnected;

public:
    SocketClient();
    virtual ~SocketClient();

    void InitSocket();
    int Connect(const char* ip, unsigned short port);
    void Close();

    bool OnRun();
    bool isRun();
    int RecvData(SOCKET _cSock);
    void OnNetMsg(DataHeader* header);
    int SendData(DataHeader* header,int nLen);

};


#endif //SOCKET_SOCKETCLIENT_H
