//
// Created by ligang on 2020/6/23.
//

#define  WIN32_LEAN_AND_MEAN //主要解决WinSock2.h头文件引入问题

#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include <inaddr.h>

#include "../DataStruct.h"
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


int main() {
    WORD ver=MAKEWORD(2,2);//版本号
    WSADATA dat;
    WSAStartup(ver,&dat);//启动windows的socket 2.x网络环境
    ///1）建立一个socket套接字
    SOCKET _sock=socket(AF_INET,SOCK_STREAM,0);//  ipv4网络，流类型，tcp/udp
    if(INVALID_SOCKET==_sock){
        std::cout<<"ERROR-->build socket failed!"<<std::endl;
    }
    ///2)connect连接服务器
    sockaddr_in _sin={};
    _sin.sin_family=AF_INET;
    _sin.sin_port=htons(9999);//host to net unsigned short
    _sin.sin_addr.S_un.S_addr= inet_addr("127.0.0.1");
    int ret=connect(_sock,(sockaddr*)&_sin, sizeof(sockaddr_in));
    if(SOCKET_ERROR==ret){
        std::cout<<"ERROR-->connect failed!"<<std::endl;
    }else{
        std::cout<<"success-->connect success!"<<std::endl;
    }

    while(true){
        fd_set fdReads;
        FD_ZERO(&fdReads);
        FD_SET(_sock,&fdReads);
        timeval t={1,0};
        int ret=select(_sock,&fdReads,0,0,&t);
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
        Login login;
        strcpy(login.userName,"ligang");
        strcpy(login.PassWord,"123456");
        send(_sock,(const char*)&login, sizeof(Login),0);
        std::cout<<"执行其他任务..."<<std::endl;
        Sleep(1000);//1000ms
    }
    ///7)关闭套接字
    closesocket(_sock);
    ///
    WSACleanup();//清除Win Socket环境
    std::cout<<"已退出，任务结束"<<std::endl;
    return 0;
}