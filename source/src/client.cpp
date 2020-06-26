//
// Created by ligang on 2020/6/23.
//

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



int processor(SOCKET _cSock){
    char szRecv[1024]={};//接收缓冲区
    int nLen=recv(_cSock,(char*)&szRecv, sizeof(DataHeader),0);//数据先接收包头大小
    DataHeader* header=(DataHeader*)szRecv;
    if(nLen<=0){
        std::cout<<"与服务器断开连接~~~~~"<<std::endl;
        return -1;
    }

    ///6）处理请求
    switch (header->cmd){
        case CMD_LOGIN_RESULT:
        {
            recv(_cSock,szRecv+ sizeof(DataHeader), header->dataLength- sizeof(DataHeader),0);
            LoginResult* loginResult=(LoginResult*)szRecv;
            std::cout<<"收到服务端消息：CMD_LOGIN_RESULT,数据长度："<<loginResult->dataLength
                     <<" ,内容: "<<loginResult->result<<std::endl;
        }
            break;
        case CMD_LOGOUT_RESULT:
        {
            recv(_cSock,szRecv+ sizeof(DataHeader), header->dataLength- sizeof(DataHeader),0);
            LogoutResult* logoutResult=(LogoutResult*)szRecv;
            std::cout<<"收到服务端消息：CMD_LOGOUT_RESULT,数据长度："<<logoutResult->dataLength
                     <<" ,内容: "<<logoutResult->result<<std::endl;
        }
            break;
        case CMD_NEW_USER_JOIN:
        {
            recv(_cSock,szRecv+ sizeof(DataHeader), header->dataLength- sizeof(DataHeader),0);
            NewUserJoin* userJoin=(NewUserJoin*)szRecv;
            std::cout<<"收到服务端消息：CMD_NEW_USER_JOIN,数据长度："<<userJoin->dataLength
                     <<" ,内容: "<<userJoin->sock<<std::endl;
        }
            break;
        default:
            break;
    }
    return 0;
}

bool g_bRun=true;
///子线程处理函数
void cmdThread(SOCKET sock){
    while (true){
        char cmdBuf[256]={};
        scanf("%s",cmdBuf);
        if(0==strcmp(cmdBuf,"exit")){
            g_bRun=false;
            std::cout<<"退出cmdThread线程！"<<std::endl;
            break;
        }else if(0==strcmp(cmdBuf,"login")){
            Login login;
            strcpy(login.userName,"ligang");
            strcpy(login.PassWord,"123456");
            send(sock,(const char*)&login, sizeof(Login),0);
        }else if(0==strcmp(cmdBuf,"logout")){
            Logout logout;
            strcpy(logout.userName,"ligang");
            send(sock,(const char*)&logout, sizeof(Logout),0);
        }else{
            std::cout<<"不支持该命令！"<<std::endl;
        }
    }
}


int main() {
#ifdef _WIN32
    WORD ver=MAKEWORD(2,2);//版本号
    WSADATA dat;
    WSAStartup(ver,&dat);//启动windows的socket 2.x网络环境
#endif
    ///1）建立一个socket套接字
    SOCKET _sock=socket(AF_INET,SOCK_STREAM,0);//  ipv4网络，流类型，tcp/udp
    if(INVALID_SOCKET==_sock){
        std::cout<<"ERROR-->build socket failed!"<<std::endl;
    }
    ///2)connect连接服务器
    sockaddr_in _sin={};
    _sin.sin_family=AF_INET;
    _sin.sin_port=htons(9999);//host to net unsigned short

#ifdef  _WIN32
    _sin.sin_addr.S_un.S_addr= inet_addr("127.0.0.1");
#else
    _sin.sin_addr.s_addr= inet_addr("127.0.0.1");
#endif
    int ret=connect(_sock,(sockaddr*)&_sin, sizeof(sockaddr_in));
    if(SOCKET_ERROR==ret){
        std::cout<<"ERROR-->connect failed!"<<std::endl;
    }else{
        std::cout<<"success-->connect success!"<<std::endl;
    }

    ///启动线程
    std::thread t1(cmdThread,_sock);
    t1.detach();//Detach 线程。 将当前线程对象所代表的执行实例与该线程对象分离，使得线程的执行可以单独进行。
    // 一旦线程执行完毕，它所分配的资源将会被释放。
    //上面的意思就是，使用detach,main函数不用等待线程结束才能结束，有时候线程还没有结束，main函数就已经结束了。

    while(g_bRun){
        fd_set fdReads;
        FD_ZERO(&fdReads);
        FD_SET(_sock,&fdReads);
        timeval t={1,0};
        int ret=select(_sock+1,&fdReads,0,0,&t);
        if(ret<0){
            std::cout<<"select任务结束"<<std::endl;
            break;
        }
        if(FD_ISSET(_sock,&fdReads)){
            FD_CLR(_sock,&fdReads);

            if(-1==processor(_sock)){
                std::cout<<"select任务结束2"<<std::endl;
                break;
            }
        }

        ///线程thread
//        std::cout<<"执行其他任务..."<<std::endl;
//        Sleep(1000);//1000ms
    }
    ///7)关闭套接字
#ifdef  _WIN32
    closesocket(_sock);
    ///
    WSACleanup();//清除Win Socket环境
#else
    close(_sock);
#endif
    
    std::cout<<"已退出，任务结束"<<std::endl;
    return 0;
}