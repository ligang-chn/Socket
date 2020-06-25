//
// Created by ligang on 2020/6/23.
//

#define  WIN32_LEAN_AND_MEAN //��Ҫ���WinSock2.hͷ�ļ���������

#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include <inaddr.h>

#include "../DataStruct.h"
int processor(SOCKET _cSock){
    char szRecv[1024]={};//���ջ�����
    int nLen=recv(_cSock,(char*)&szRecv, sizeof(DataHeader),0);//�����Ƚ��հ�ͷ��С
    DataHeader* header=(DataHeader*)szRecv;
    if(nLen<=0){
        std::cout<<"��������Ͽ�����~~~~~"<<std::endl;
        return -1;
    }

    ///6����������
    switch (header->cmd){
        case CMD_LOGIN_RESULT:
        {
            recv(_cSock,szRecv+ sizeof(DataHeader), header->dataLength- sizeof(DataHeader),0);
            LoginResult* loginResult=(LoginResult*)szRecv;
            std::cout<<"�յ��������Ϣ��CMD_LOGIN_RESULT,���ݳ��ȣ�"<<loginResult->dataLength
                     <<" ,����: "<<loginResult->result<<std::endl;
        }
            break;
        case CMD_LOGOUT_RESULT:
        {
            recv(_cSock,szRecv+ sizeof(DataHeader), header->dataLength- sizeof(DataHeader),0);
            LogoutResult* logoutResult=(LogoutResult*)szRecv;
            std::cout<<"�յ��������Ϣ��CMD_LOGOUT_RESULT,���ݳ��ȣ�"<<logoutResult->dataLength
                     <<" ,����: "<<logoutResult->result<<std::endl;
        }
            break;
        case CMD_NEW_USER_JOIN:
        {
            recv(_cSock,szRecv+ sizeof(DataHeader), header->dataLength- sizeof(DataHeader),0);
            NewUserJoin* userJoin=(NewUserJoin*)szRecv;
            std::cout<<"�յ��������Ϣ��CMD_NEW_USER_JOIN,���ݳ��ȣ�"<<userJoin->dataLength
                     <<" ,����: "<<userJoin->sock<<std::endl;
        }
            break;
        default:
            break;
    }
    return 0;
}


int main() {
    WORD ver=MAKEWORD(2,2);//�汾��
    WSADATA dat;
    WSAStartup(ver,&dat);//����windows��socket 2.x���绷��
    ///1������һ��socket�׽���
    SOCKET _sock=socket(AF_INET,SOCK_STREAM,0);//  ipv4���磬�����ͣ�tcp/udp
    if(INVALID_SOCKET==_sock){
        std::cout<<"ERROR-->build socket failed!"<<std::endl;
    }
    ///2)connect���ӷ�����
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
            std::cout<<"select�������"<<std::endl;
            break;
        }
        if(FD_ISSET(_sock,&fdReads)){
            FD_CLR(_sock,&fdReads);

            if(-1==processor(_sock)){
                std::cout<<"select�������2"<<std::endl;
                break;
            }
        }
        Login login;
        strcpy(login.userName,"ligang");
        strcpy(login.PassWord,"123456");
        send(_sock,(const char*)&login, sizeof(Login),0);
        std::cout<<"ִ����������..."<<std::endl;
        Sleep(1000);//1000ms
    }
    ///7)�ر��׽���
    closesocket(_sock);
    ///
    WSACleanup();//���Win Socket����
    std::cout<<"���˳����������"<<std::endl;
    return 0;
}