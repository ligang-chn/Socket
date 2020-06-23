//
// Created by ligang on 2020/6/23.
//

#define  WIN32_LEAN_AND_MEAN //��Ҫ���WinSock2.hͷ�ļ���������

#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include <inaddr.h>

int main() {
    WORD ver=MAKEWORD(2,2);//�汾��
    WSADATA dat;
    WSAStartup(ver,&dat);//����windows��socket 2.x���绷��
    ///
    SOCKET _sock=socket(AF_INET,SOCK_STREAM,0);//1������һ��socket�׽���  ipv4���磬�����ͣ�tcp/udp
    if(INVALID_SOCKET==_sock){
        std::cout<<"ERROR-->build socket failed!"<<std::endl;
    }
    //2)connect���ӷ�����
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
    //3)recv ���շ�������Ϣ
    char recvBuf[256]={};
    int nlen=recv(_sock,recvBuf,256,0);
    if(nlen>0){
        std::cout<<"���յ������ݣ� "<<recvBuf<<std::endl;
    }
    //4)�ر��׽���
    closesocket(_sock);

    ///
    WSACleanup();//���Win Socket����

    return 0;
}