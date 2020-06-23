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
    SOCKET _sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//1）建立一个socket套接字  ipv4网络，流类型，tcp/udp
    sockaddr_in _sin={};
    _sin.sin_family=AF_INET;
    _sin.sin_port=htons(9999);//host to net unsigned short
    _sin.sin_addr.S_un.S_addr= INADDR_ANY; //inet_addr("127.0.0.1");
    if(SOCKET_ERROR== bind(_sock,(sockaddr*)&_sin, sizeof(_sin)))//2)绑定端口
    {
        std::cout<<"ERROR-->bind failed!"<<std::endl;
    }

    if(SOCKET_ERROR== listen(_sock,5))//3)监听端口
    {
        std::cout<<"ERROR-->listen failed!"<<std::endl;
    }else{
        std::cout<<"Start Listening~~~~"<<std::endl;
    }

    //4)accept 等待接收客户端连接
    sockaddr_in clientAddr={};//存放返回的客户端地址
    int nAddrLen= sizeof(sockaddr_in);
    SOCKET _cSock=INVALID_SOCKET;

    char msgBuf[]="I'm Server.";
    _cSock=accept(_sock,(sockaddr*)&clientAddr, &nAddrLen);
    if(_cSock==INVALID_SOCKET){
        std::cout<<"ERROR-->INVALID_SOCKET!"<<std::endl;
    }
    std::cout<<"新客户端加入：IP = "<<inet_ntoa(clientAddr.sin_addr)<<std::endl;

    char _recvBuf[128]={};
    while (true){
        //5）接收客户端数据
        int nLen=recv(_cSock,_recvBuf,128,0);
        if(nLen<=0){
            std::cout<<"客户端退出~~~~~"<<std::endl;
            break;
        }
        std::cout<<"收到命令："<<_recvBuf<<std::endl;
        //6）处理请求
        if(0==strcmp(_recvBuf,"getName")){
            //7)send 向客户端发送数据
            char msgBuf[]="name: Server.";
            send(_cSock,msgBuf,strlen(msgBuf)+1,0);
        }else if(0==strcmp(_recvBuf,"getAge")){
            char msgBuf[]="80.";
            send(_cSock,msgBuf,strlen(msgBuf)+1,0);
        } else{
            char msgBuf[]="???";
            send(_cSock,msgBuf,strlen(msgBuf)+1,0);
        }
    }

    //8)关闭套接字
    closesocket(_sock);

    ///
    WSACleanup();//清除Win Socket环境
    std::cout<<"已退出，任务结束"<<std::endl;
    return 0;
}