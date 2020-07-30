//
// Created by ligang on 2020/7/22.
//

#ifndef SOCKET_SOCKETSERVER_H
#define SOCKET_SOCKETSERVER_H

#define  WIN32_LEAN_AND_MEAN //主要解决WinSock2.h头文件引入问题

#ifdef _WIN32
    #define FD_SETSIZE	2506
    #include <windows.h>
    #include <winsock2.h>
    #include <inaddr.h>//这个可能是之前clion自动引入的

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
#define  RECV_BUFF_SIZE 10240     //缓冲区最小单元大小
#endif

//#define _CellServer_THREAD_COUNT 4   //线程数量
//客户端数据类型
class ClientSocket{
public:
    ClientSocket(SOCKET sockfd = INVALID_SOCKET){
        _sockfd=sockfd;
        memset(_szMsgBuf,0, sizeof(_szMsgBuf)); //第二缓冲区赋初值
        _lastPos=0; //尾部位置初始化
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
    //发送数据
    int SendData(DataHeader *header) {
        if(header){
            return send(_sockfd,(const char*)header, header->dataLength,0);
        }
        return SOCKET_ERROR;
    }
private:
    SOCKET _sockfd; //fd_set  file desc set

    char _szMsgBuf[RECV_BUFF_SIZE*5]; //第二/消息缓冲区
    int _lastPos;  //消息缓冲区的数据尾部位置
};

//网络事件接口
class INetEvent{
public:
    //客户端加入事件
    virtual void OnNetJoin(ClientSocket* pClient)=0;
    //客户端离开事件
    virtual void OnNetLeave(ClientSocket* pClient)=0; //纯虚函数
    //客户端消息事件
    virtual void OnNetMsg(ClientSocket* pClient, DataHeader *header)=0;
};


class CellServer{
public:
    CellServer(SOCKET sock=INVALID_SOCKET);
    ~CellServer();

    void setEventObj(INetEvent* event);

    //关闭socket
    void Close();

    //是否工作中
    bool isRun();

    bool OnRun();

    //接收数据 处理粘包 拆分包
    int RecvData(ClientSocket* pClient);

    //响应网络消息
    virtual void OnNetMsg(ClientSocket* pClient, DataHeader* header);

    void addClient(ClientSocket* pClient);

    void Start();

    //查询客户端数量
    size_t getClientCount();

private:
    SOCKET _sock;
    //正式客户队列
    std::vector<ClientSocket*> _clients;
    //缓冲客户队列
    std::vector<ClientSocket*> _clientsBuf;
    char _szRecv[RECV_BUFF_SIZE]={}; //接收缓冲区
    //缓冲队列的锁
    std::mutex _mutex;
    std::thread _thread;
    //网络事件对象
    INetEvent* _pINetEvent;

};


class SocketServer :public INetEvent{
private:
    SOCKET _sock;
    std::vector<CellServer*> _cellServers;  //消息处理对象，内部会创建线程

    char _szRecv[RECV_BUFF_SIZE]={}; //接收缓冲区
    //每秒消息计时
    CELLTimestamp _tTime;
    //收到消息计数
    std::atomic_int _recvCount;
    //客户端计数
    std::atomic_int _clientCount;


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

    //启动线程
    void Start(int nCellServer);

    void addClientToServer(ClientSocket* pClient);

    //关闭socket
    void Close();

    //处理网络消息
    bool OnRun();

    //是否工作中
    bool isRun();

    //接收数据 处理粘包 拆分包
    int RecvData(ClientSocket* pClient);

    //响应网络消息
    void time4msg();

    //发送指定Socket数据
//    int SendData(SOCKET _cSock, DataHeader* header);

    //群发消息
//    void SendDataToAll(DataHeader* header);
    virtual void OnNetJoin(ClientSocket* pClient);
    virtual void OnNetLeave(ClientSocket* pClient); //纯虚函数
    virtual void OnNetMsg(ClientSocket* pClient, DataHeader *header);

};


#endif //SOCKET_SOCKETSERVER_H
