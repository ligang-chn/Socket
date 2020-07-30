//
// Created by ligang on 2020/6/23.
//
#if 1

#include <iostream>
#include "SocketServer.h"

using namespace std;

bool g_bRun=true;
///子线程处理函数
void cmdThread(){
    while (true){   //这个线程一直阻塞在scanf处，等待命令输入
        char cmdBuf[256]={};
        scanf("%s",cmdBuf);
        if(0==strcmp(cmdBuf,"exit")) {
            g_bRun=false;
            std::cout << "退出cmdThread线程！" << std::endl;
            break;
//        }else if(0==strcmp(cmdBuf,"login")){
//            Login login;
//            strcpy(login.userName,"ligang");
//            strcpy(login.PassWord,"123456");
//            client->SendData(&login);
//        }else if(0==strcmp(cmdBuf,"logout")){
//            Logout logout;
//            strcpy(logout.userName,"ligang");
//            client->SendData(&logout);
        }else{
            std::cout<<"不支持该命令！"<<std::endl;
        }
    }
}

int main(){

    SocketServer server;
    server.InitSocket();
    server.Bind(nullptr,9999);
    server.Listen(5);
    server.Start();
//    SocketServer server1;
//    server1.InitSocket();
//    server1.Bind(nullptr,9998);
//    server1.Listen(5);
    ///启动线程
    std::thread t1(cmdThread);
    t1.detach();//Detach 线程。 将当前线程对象所代表的执行实例与该线程对象分离，使得线程的执行可以单独进行。
    // 一旦线程执行完毕，它所分配的资源将会被释放。
    //上面的意思就是，使用detach,main函数不用等待线程结束才能结束，有时候线程还没有结束，main函数就已经结束了。

    while (g_bRun){ //||server1.isRun()
        server.OnRun();   //等待客户端连接——生产者线程
//        server1.OnRun();
    }
    server.Close();
//    server1.Close();
    cout<<"已退出"<<endl;
    return 0;
}


#endif

#if 0
#define  WIN32_LEAN_AND_MEAN //主要解决WinSock2.h头文件引入问题


#ifdef _WIN32
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
#include <vector>
#include <algorithm>

std::vector<SOCKET> g_clients;

int processor(SOCKET _cSock){
    char szRecv[1024]={};//接收缓冲区
    int nLen=(int)recv(_cSock,(char*)&szRecv, sizeof(DataHeader),0);//数据先接收包头大小
    DataHeader* header=(DataHeader*)szRecv;
    if(nLen<=0){
        std::cout<<_cSock<<" 客户端退出~~~~~"<<std::endl;
        return -1;
    }

    ///6）处理请求
    switch (header->cmd){
        case CMD_LOGIN:
        {
            recv(_cSock,szRecv+ sizeof(DataHeader), header->dataLength- sizeof(DataHeader),0);
            Login* login=(Login*)szRecv;
            std::cout<<"socket: "<<_cSock<<" 收到命令：CMD_LOGIN ,数据长度："<<login->dataLength
                     <<" ,username: "<<login->userName<<" ,password: "<<login->PassWord<<std::endl;
            //忽略判断用户密码是否正确的过程
            LoginResult ret;
            send(_cSock,(char*)&ret, sizeof(LoginResult),0);
        }
            break;
        case CMD_LOGOUT:
        {
            recv(_cSock,szRecv+ sizeof(DataHeader), header->dataLength- sizeof(DataHeader),0);
            Logout* logout=(Logout*)szRecv;
            std::cout<<"socket: "<<_cSock<<" 收到命令：CMD_LOGIN ,数据长度："<<logout->dataLength
                     <<" ,username: "<<logout->userName<<std::endl;
            //忽略判断用户密码是否正确的过程
            LogoutResult ret;
            send(_cSock,(char*)&ret, sizeof(LogoutResult),0);
        }
            break;
        default:
        {
            DataHeader header={CMD_ERROR,0};
            send(_cSock,(char*)&header, sizeof(DataHeader),0);
        }

            break;
    }
    return 0;
}

int main() {
#ifdef _WIN32
    WORD ver=MAKEWORD(2,2);//版本号
    WSADATA dat;
    WSAStartup(ver,&dat);//启动windows的socket 2.x网络环境
#endif

    ///1）建立一个socket套接字
    SOCKET _sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//  ipv4网络，流类型，tcp/udp
    sockaddr_in _sin={};
    _sin.sin_family=AF_INET;
    _sin.sin_port=htons(9999);//host to net unsigned short
#ifdef _WIN32
    _sin.sin_addr.S_un.S_addr= INADDR_ANY; //inet_addr("127.0.0.1");
#else
    _sin.sin_addr.s_addr= INADDR_ANY;
#endif
    ///2)绑定端口
    if(SOCKET_ERROR== bind(_sock,(sockaddr*)&_sin, sizeof(_sin)))
    {
        std::cout<<"ERROR-->bind failed!"<<std::endl;
    }
    ///3)监听端口
    if(SOCKET_ERROR== listen(_sock,5))
    {
        std::cout<<"ERROR-->listen failed!"<<std::endl;
    }else{
        std::cout<<"Start Listening~~~~"<<std::endl;
    }


    while (true){
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExp;

        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExp);

        FD_SET(_sock,&fdRead);
        FD_SET(_sock,&fdWrite);
        FD_SET(_sock,&fdExp);

        SOCKET maxSock=_sock;//计算最大描述符
        for(int n=(int)g_clients.size()-1;n>=0;n--){
            FD_SET(g_clients[n],&fdRead);
            if(maxSock<g_clients[n]){
                maxSock=g_clients[n];
            }
        }
        ///nfds是一个整数值，是指fd_set集合中所有描述符（socket）的范围，而不是数量
        ///即所有文件描述符最大值+1，在windows中这个参数无所谓，可以写0
        timeval t={1,0};//非阻塞模式
        int ret=select(maxSock+1,&fdRead,&fdWrite,&fdExp, &t);
        if(ret<0){
            std::cout<<"select任务结束~~~~~"<<std::endl;
            break;
        }
        if(FD_ISSET(_sock,&fdRead)){
            FD_CLR(_sock,&fdRead);
            ///4)accept 等待接收客户端连接
            sockaddr_in clientAddr={};//存放返回的客户端地址
            int nAddrLen= sizeof(sockaddr_in);
            SOCKET _cSock=INVALID_SOCKET;
#ifdef _WIN32
            _cSock=accept(_sock,(sockaddr*)&clientAddr, &nAddrLen);
#else
            _cSock=accept(_sock,(sockaddr*)&clientAddr, (socklen_t*)&nAddrLen);
#endif
            if(_cSock==INVALID_SOCKET){
                std::cout<<"ERROR-->INVALID_SOCKET!"<<std::endl;
            }else{
                for(int n=(int)g_clients.size()-1;n>=0;n--){
                    NewUserJoin userJoin((int)_cSock);
                    send(g_clients[n],(const char*)&userJoin, sizeof(NewUserJoin),0);
                }
                g_clients.push_back(_cSock);
                std::cout<<"新客户端加入：IP = "<<inet_ntoa(clientAddr.sin_addr)<<" ,Port= "<<(int)_cSock<<std::endl;
            }
        }


        ///5）接收客户端数据
#ifdef _WIN32
        for(int n=0;n<fdRead.fd_count;n++){
            if(-1==processor(fdRead.fd_array[n])){
                auto iter=std::find(g_clients.begin(),g_clients.end(),fdRead.fd_array[n]);
                if(iter!=g_clients.end()){
                    g_clients.erase(iter);
                }
            }
        }
#else
        for(int n=(int)g_clients.size()-1;n>=0;n--){
            if(FD_ISSET(g_clients[n],&fdRead)){
                if(-1==processor(g_clients[n])){
                    auto iter=g_clients.begin()+n;
                    if(iter!=g_clients.end()){
                        g_clients.erase(iter);
                    }
                }
            }
        }
#endif
        // std::cout<<"执行其他任务..."<<std::endl;

    }
#ifdef _WIN32
    for(size_t n=g_clients.size()-1;n>=0;n--){
        closesocket(g_clients[n]);
    }
    ///8)关闭套接字
    closesocket(_sock);
    ///
    WSACleanup();//清除Win Socket环境
#else
    for(int n=(int)g_clients.size()-1;n>=0;n--){
        close(g_clients[n]);
    }
    ///8)关闭套接字
    close(_sock);
#endif
    std::cout<<"已退出，任务结束"<<std::endl;
    return 0;
}
#endif


