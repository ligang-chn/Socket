//
// Created by ligang on 2020/6/26.
//

#ifndef SOCKET_SOCKETCLIENT_H
#define SOCKET_SOCKETCLIENT_H

#define  WIN32_LEAN_AND_MEAN //��Ҫ���WinSock2.hͷ�ļ���������

#ifdef _WIN32
    #define FD_SETSIZE 1024
    #include <windows.h>
    #include <WinSock2.h>
    #include <inaddr.h>//���������֮ǰclion�Զ������

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
#define  RECV_BUFF_SIZE 10240     //��������С��Ԫ��С
#endif

class SocketClient {
private:
    SOCKET _sock;
    char _szRecv[RECV_BUFF_SIZE]={}; //���ջ�����
    char _szMsgBuf[RECV_BUFF_SIZE*10]={}; //�ڶ�/��Ϣ������
    int _lastPos=0;  //��Ϣ������������β��λ��
    int _nCount=0;

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
    int SendData(DataHeader* header);

};


#endif //SOCKET_SOCKETCLIENT_H
