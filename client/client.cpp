//
// Created by ligang on 2020/6/23.
//

#define  WIN32_LEAN_AND_MEAN //主要解决WinSock2.h头文件引入问题

#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include <inaddr.h>

int main() {
    WORD ver=MAKEWORD(2,2);//版本号
    WSADATA dat;
    WSAStartup(ver,&dat);//启动windows的socket 2.x网络环境
    ///
    SOCKET _sock=socket(AF_INET,SOCK_STREAM,0);//1）建立一个socket套接字  ipv4网络，流类型，tcp/udp
    if(INVALID_SOCKET==_sock){
        std::cout<<"ERROR-->build socket failed!"<<std::endl;
    }
    //2)connect连接服务器
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
    //3)recv 接收服务器信息
    char recvBuf[256]={};
    int nlen=recv(_sock,recvBuf,256,0);
    if(nlen>0){
        std::cout<<"接收到的数据： "<<recvBuf<<std::endl;
    }
    //4)关闭套接字
    closesocket(_sock);

    ///
    WSACleanup();//清除Win Socket环境

    return 0;
}