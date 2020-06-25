//
// Created by ligang on 2020/6/23.
//

#define  WIN32_LEAN_AND_MEAN //��Ҫ���WinSock2.hͷ�ļ���������

#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include <inaddr.h>

#include "../DataStruct.h"

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
        ///3��������������
        char cmdBuf[128]={};//���ڴ洢��������
        scanf("%s",&cmdBuf);
        ///4��������������
        if(0==strcmp(cmdBuf,"exit")){
            break;
        }else if(0==strcmp(cmdBuf,"login")){//�������������ֻ�ǿͻ�����������Ҫ���͵�����
            Login login;//�������ݻ�����
            strcpy(login.userName,"ligang");
            strcpy(login.PassWord,"123456");
            ///5���������������������
            send(_sock,(const char*)&login, sizeof(Login),0);
            //���շ��������ص�����
            LoginResult loginRet={};
            recv(_sock,(char*)&loginRet, sizeof(LoginResult),0);

            std::cout<<"LoginResult: "<<loginRet.result<<std::endl;

        }else if(0==strcmp(cmdBuf,"logout")){
            Logout logout;
            strcpy(logout.userName,"ligang");
            ///5���������������������
            send(_sock,(const char*)&logout, sizeof(Logout),0);
            //���շ��������ص�����
            LogoutResult logoutRet={};
            recv(_sock,(char*)&logoutRet, sizeof(LogoutResult),0);

            std::cout<<"LoginResult: "<<logoutRet.result<<std::endl;

        }else{
            std::cout<<"��֧�ֵ�����~"<<std::endl;
        }
    }
    ///7)�ر��׽���
    closesocket(_sock);
    ///
    WSACleanup();//���Win Socket����
    std::cout<<"���˳����������"<<std::endl;
    return 0;
}