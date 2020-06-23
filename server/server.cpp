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
    SOCKET _sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//1������һ��socket�׽���  ipv4���磬�����ͣ�tcp/udp
    sockaddr_in _sin={};
    _sin.sin_family=AF_INET;
    _sin.sin_port=htons(9999);//host to net unsigned short
    _sin.sin_addr.S_un.S_addr= INADDR_ANY; //inet_addr("127.0.0.1");
    if(SOCKET_ERROR== bind(_sock,(sockaddr*)&_sin, sizeof(_sin)))//2)�󶨶˿�
    {
        std::cout<<"ERROR-->bind failed!"<<std::endl;
    }

    if(SOCKET_ERROR== listen(_sock,5))//3)�����˿�
    {
        std::cout<<"ERROR-->listen failed!"<<std::endl;
    }else{
        std::cout<<"Start Listening~~~~"<<std::endl;
    }

    //4)accept �ȴ����տͻ�������
    sockaddr_in clientAddr={};//��ŷ��صĿͻ��˵�ַ
    int nAddrLen= sizeof(sockaddr_in);
    SOCKET _cSock=INVALID_SOCKET;

    char msgBuf[]="I'm Server.";

    while (true){
        _cSock=accept(_sock,(sockaddr*)&clientAddr, &nAddrLen);
        if(_cSock==INVALID_SOCKET){
            std::cout<<"ERROR-->INVALID_SOCKET!"<<std::endl;
        }
        std::cout<<"�¿ͻ��˼��룺IP = "<<inet_ntoa(clientAddr.sin_addr)<<std::endl;
        //5)send ��ͻ��˷�������
        send(_cSock,msgBuf,strlen(msgBuf)+1,0);
    }

    //6)�ر��׽���
    closesocket(_sock);

    ///
    WSACleanup();//���Win Socket����

    return 0;
}