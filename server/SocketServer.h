//
// Created by ligang on 2020/7/22.
//

#ifndef SOCKET_SOCKETSERVER_H
#define SOCKET_SOCKETSERVER_H

#define  WIN32_LEAN_AND_MEAN //��Ҫ���WinSock2.hͷ�ļ���������

#ifdef _WIN32
    #include <windows.h>
    #include <winsock2.h>
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
#include <vector>
#include <algorithm>

#ifndef  RECV_BUFF_SIZE
#define  RECV_BUFF_SIZE 10240     //��������С��Ԫ��С
#endif

class ClientSocket{
public:
    ClientSocket(SOCKET sockfd = INVALID_SOCKET){
        _sockfd=sockfd;
        memset(_szMsgBuf,0, sizeof(_szMsgBuf));
        _lastPos=0;
    }

    SOCKET sockfd(){
        return _sockfd;
    }

    char* msgBuf(){
        return _szMsgBuf;
    }

    int getLastPos(){
        return _lastPos;
    }

    void setLastPos(int pos){
        _lastPos=pos;
    }
private:
    SOCKET _sockfd; //fd_set  file desc set

    char _szMsgBuf[RECV_BUFF_SIZE*10]; //�ڶ�/��Ϣ������
    int _lastPos;  //��Ϣ������������β��λ��
};

class SocketServer {
private:
    SOCKET _sock;
    std::vector<ClientSocket*> _clients;
    char _szRecv[RECV_BUFF_SIZE]={}; //���ջ�����

public:
    SocketServer();
    virtual ~SocketServer();

    //��ʼ��socket
    SOCKET InitSocket();

    //�󶨶˿ں�
    int Bind(const char* ip, unsigned short port);

    //�����˿�
    int Listen(int n);

    //���ܿͻ�������
    SOCKET Accept();
    //�ر�socket
    void Close();

    //����������Ϣ
    bool OnRun();

    //�Ƿ�����
    bool isRun();

    //�������� ����ճ�� ��ְ�
    int RecvData(ClientSocket* pClient);

    //��Ӧ������Ϣ
    virtual void OnNetMsg(SOCKET _cSock, DataHeader* header);

    //����ָ��Socket����
    int SendData(SOCKET _cSock, DataHeader* header);

    //Ⱥ����Ϣ
    void SendDataToAll(DataHeader* header);
};


#endif //SOCKET_SOCKETSERVER_H
