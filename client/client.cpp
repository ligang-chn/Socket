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
        ///3）输入请求命令
        char cmdBuf[128]={};//用于存储请求命令
        scanf("%s",&cmdBuf);
        ///4）处理请求命令
        if(0==strcmp(cmdBuf,"exit")){
            break;
        }else if(0==strcmp(cmdBuf,"login")){//这里的请求命令只是客户端用来区分要发送的数据
            Login login;//发送数据缓冲区
            strcpy(login.userName,"ligang");
            strcpy(login.PassWord,"123456");
            ///5）向服务器发送请求命令
            send(_sock,(const char*)&login, sizeof(Login),0);
            //接收服务器返回的数据
            LoginResult loginRet={};
            recv(_sock,(char*)&loginRet, sizeof(LoginResult),0);

            std::cout<<"LoginResult: "<<loginRet.result<<std::endl;

        }else if(0==strcmp(cmdBuf,"logout")){
            Logout logout;
            strcpy(logout.userName,"ligang");
            ///5）向服务器发送请求命令
            send(_sock,(const char*)&logout, sizeof(Logout),0);
            //接收服务器返回的数据
            LogoutResult logoutRet={};
            recv(_sock,(char*)&logoutRet, sizeof(LogoutResult),0);

            std::cout<<"LoginResult: "<<logoutRet.result<<std::endl;

        }else{
            std::cout<<"不支持的命令~"<<std::endl;
        }
    }
    ///7)关闭套接字
    closesocket(_sock);
    ///
    WSACleanup();//清除Win Socket环境
    std::cout<<"已退出，任务结束"<<std::endl;
    return 0;
}