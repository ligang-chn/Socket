//
// Created by ligang on 2020/7/22.
//

#ifndef SOCKET_SOCKETSERVER_H
#define SOCKET_SOCKETSERVER_H

#define  WIN32_LEAN_AND_MEAN //��Ҫ���WinSock2.hͷ�ļ���������

#ifdef _WIN32
    #define FD_SETSIZE	2506
    #include <windows.h>
    #include <winsock2.h>
    #include <inaddr.h>//���������֮ǰclion�Զ������

    #include "../DataStruct.h"
    #include "../ext/CELLTimestamp.h"
#else
    #include <unistd.h>//uni std
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <string.h>
    #include <stdlib.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include "./DataStruct.h"
    #include "./ext/CELLTimestamp.h"

    #define SOCKET int
    #define INVALID_SOCKET (SOCKET)(~0)
    #define SOCKET_ERROR           (-1)
#endif

#include <iostream>
#include <stdlib.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <mutex>
#include <atomic>

#ifndef  RECV_BUFF_SIZE
#define  RECV_BUFF_SIZE 10240     //��������С��Ԫ��С
#endif

//#define _CellServer_THREAD_COUNT 4   //�߳�����
//�ͻ�����������
class ClientSocket{
public:
    ClientSocket(SOCKET sockfd = INVALID_SOCKET){
        _sockfd=sockfd;
        memset(_szMsgBuf,0, sizeof(_szMsgBuf)); //�ڶ�����������ֵ
        _lastPos=0; //β��λ�ó�ʼ��
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
    //��������
    int SendData(DataHeader *header) {
        if(header){
            return send(_sockfd,(const char*)header, header->dataLength,0);
        }
        return SOCKET_ERROR;
    }
private:
    SOCKET _sockfd; //fd_set  file desc set

    char _szMsgBuf[RECV_BUFF_SIZE*5]; //�ڶ�/��Ϣ������
    int _lastPos;  //��Ϣ������������β��λ��
};

//�����¼��ӿ�
class INetEvent{
public:
    //�ͻ��˼����¼�
    virtual void OnNetJoin(ClientSocket* pClient)=0;
    //�ͻ����뿪�¼�
    virtual void OnNetLeave(ClientSocket* pClient)=0; //���麯��
    //�ͻ�����Ϣ�¼�
    virtual void OnNetMsg(ClientSocket* pClient, DataHeader *header)=0;
};


class CellServer{
public:
    CellServer(SOCKET sock=INVALID_SOCKET);
    ~CellServer();

    void setEventObj(INetEvent* event);

    //�ر�socket
    void Close();

    //�Ƿ�����
    bool isRun();

    bool OnRun();

    //�������� ����ճ�� ��ְ�
    int RecvData(ClientSocket* pClient);

    //��Ӧ������Ϣ
    virtual void OnNetMsg(ClientSocket* pClient, DataHeader* header);

    void addClient(ClientSocket* pClient);

    void Start();

    //��ѯ�ͻ�������
    size_t getClientCount();

private:
    SOCKET _sock;
    //��ʽ�ͻ�����
    std::vector<ClientSocket*> _clients;
    //����ͻ�����
    std::vector<ClientSocket*> _clientsBuf;
    char _szRecv[RECV_BUFF_SIZE]={}; //���ջ�����
    //������е���
    std::mutex _mutex;
    std::thread _thread;
    //�����¼�����
    INetEvent* _pINetEvent;

};


class SocketServer :public INetEvent{
private:
    SOCKET _sock;
    std::vector<CellServer*> _cellServers;  //��Ϣ��������ڲ��ᴴ���߳�

    char _szRecv[RECV_BUFF_SIZE]={}; //���ջ�����
    //ÿ����Ϣ��ʱ
    CELLTimestamp _tTime;
    //�յ���Ϣ����
    std::atomic_int _recvCount;
    //�ͻ��˼���
    std::atomic_int _clientCount;


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

    //�����߳�
    void Start(int nCellServer);

    void addClientToServer(ClientSocket* pClient);

    //�ر�socket
    void Close();

    //����������Ϣ
    bool OnRun();

    //�Ƿ�����
    bool isRun();

    //�������� ����ճ�� ��ְ�
    int RecvData(ClientSocket* pClient);

    //��Ӧ������Ϣ
    void time4msg();

    //����ָ��Socket����
//    int SendData(SOCKET _cSock, DataHeader* header);

    //Ⱥ����Ϣ
//    void SendDataToAll(DataHeader* header);
    virtual void OnNetJoin(ClientSocket* pClient);
    virtual void OnNetLeave(ClientSocket* pClient); //���麯��
    virtual void OnNetMsg(ClientSocket* pClient, DataHeader *header);

};


#endif //SOCKET_SOCKETSERVER_H
