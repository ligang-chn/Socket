//
// Created by ligang on 2020/6/23.
//

#define  WIN32_LEAN_AND_MEAN //主要解决WinSock2.h头文件引入问题

#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include <inaddr.h>

#include "../DataStruct.h"


int main() {
    WORD ver=MAKEWORD(2,2);//版本号
    WSADATA dat;
    WSAStartup(ver,&dat);//启动windows的socket 2.x网络环境
    ///1）建立一个socket套接字
    SOCKET _sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//  ipv4网络，流类型，tcp/udp
    sockaddr_in _sin={};
    _sin.sin_family=AF_INET;
    _sin.sin_port=htons(9999);//host to net unsigned short
    _sin.sin_addr.S_un.S_addr= INADDR_ANY; //inet_addr("127.0.0.1");
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

    ///4)accept 等待接收客户端连接
    sockaddr_in clientAddr={};//存放返回的客户端地址
    int nAddrLen= sizeof(sockaddr_in);
    SOCKET _cSock=INVALID_SOCKET;

//    char msgBuf[]="I'm Server.";
    _cSock=accept(_sock,(sockaddr*)&clientAddr, &nAddrLen);
    if(_cSock==INVALID_SOCKET){
        std::cout<<"ERROR-->INVALID_SOCKET!"<<std::endl;
    }
    std::cout<<"新客户端加入：IP = "<<inet_ntoa(clientAddr.sin_addr)<<" ,Port= "<<clientAddr.sin_port<<std::endl;

    while (true){
        ///5）接收客户端数据
        char szRecv[1024]={};//接收缓冲区
        int nLen=recv(_cSock,(char*)&szRecv, sizeof(DataHeader),0);//数据先接收包头大小
        DataHeader* header=(DataHeader*)szRecv;
        if(nLen<=0){
            std::cout<<"客户端退出~~~~~"<<std::endl;
            break;
        }

        ///6）处理请求
        switch (header->cmd){
            case CMD_LOGIN:
                {
                    recv(_cSock,szRecv+ sizeof(DataHeader), header->dataLength- sizeof(DataHeader),0);
                    Login* login=(Login*)szRecv;
                    std::cout<<"收到命令：CMD_LOGIN ,数据长度："<<login->dataLength
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
                    std::cout<<"收到命令：CMD_LOGIN ,数据长度："<<logout->dataLength
                             <<" ,username: "<<logout->userName<<std::endl;
                    //忽略判断用户密码是否正确的过程
                    LogoutResult ret;
                    send(_cSock,(char*)&ret, sizeof(LogoutResult),0);
                }
                break;
            default:
                DataHeader header={CMD_ERROR,0};
                send(_cSock,(char*)&header, sizeof(DataHeader),0);
                break;
        }
    }
    ///8)关闭套接字
    closesocket(_sock);
    ///
    WSACleanup();//清除Win Socket环境
    std::cout<<"已退出，任务结束"<<std::endl;
    return 0;
}