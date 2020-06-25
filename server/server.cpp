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
    SOCKET _sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//  ipv4���磬�����ͣ�tcp/udp
    sockaddr_in _sin={};
    _sin.sin_family=AF_INET;
    _sin.sin_port=htons(9999);//host to net unsigned short
    _sin.sin_addr.S_un.S_addr= INADDR_ANY; //inet_addr("127.0.0.1");
    ///2)�󶨶˿�
    if(SOCKET_ERROR== bind(_sock,(sockaddr*)&_sin, sizeof(_sin)))
    {
        std::cout<<"ERROR-->bind failed!"<<std::endl;
    }
    ///3)�����˿�
    if(SOCKET_ERROR== listen(_sock,5))
    {
        std::cout<<"ERROR-->listen failed!"<<std::endl;
    }else{
        std::cout<<"Start Listening~~~~"<<std::endl;
    }

    ///4)accept �ȴ����տͻ�������
    sockaddr_in clientAddr={};//��ŷ��صĿͻ��˵�ַ
    int nAddrLen= sizeof(sockaddr_in);
    SOCKET _cSock=INVALID_SOCKET;

//    char msgBuf[]="I'm Server.";
    _cSock=accept(_sock,(sockaddr*)&clientAddr, &nAddrLen);
    if(_cSock==INVALID_SOCKET){
        std::cout<<"ERROR-->INVALID_SOCKET!"<<std::endl;
    }
    std::cout<<"�¿ͻ��˼��룺IP = "<<inet_ntoa(clientAddr.sin_addr)<<" ,Port= "<<clientAddr.sin_port<<std::endl;

    while (true){
        ///5�����տͻ�������
        char szRecv[1024]={};//���ջ�����
        int nLen=recv(_cSock,(char*)&szRecv, sizeof(DataHeader),0);//�����Ƚ��հ�ͷ��С
        DataHeader* header=(DataHeader*)szRecv;
        if(nLen<=0){
            std::cout<<"�ͻ����˳�~~~~~"<<std::endl;
            break;
        }

        ///6����������
        switch (header->cmd){
            case CMD_LOGIN:
                {
                    recv(_cSock,szRecv+ sizeof(DataHeader), header->dataLength- sizeof(DataHeader),0);
                    Login* login=(Login*)szRecv;
                    std::cout<<"�յ����CMD_LOGIN ,���ݳ��ȣ�"<<login->dataLength
                                <<" ,username: "<<login->userName<<" ,password: "<<login->PassWord<<std::endl;
                    //�����ж��û������Ƿ���ȷ�Ĺ���
                    LoginResult ret;
                    send(_cSock,(char*)&ret, sizeof(LoginResult),0);
                }
                break;
            case CMD_LOGOUT:
                {
                    recv(_cSock,szRecv+ sizeof(DataHeader), header->dataLength- sizeof(DataHeader),0);
                    Logout* logout=(Logout*)szRecv;
                    std::cout<<"�յ����CMD_LOGIN ,���ݳ��ȣ�"<<logout->dataLength
                             <<" ,username: "<<logout->userName<<std::endl;
                    //�����ж��û������Ƿ���ȷ�Ĺ���
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
    ///8)�ر��׽���
    closesocket(_sock);
    ///
    WSACleanup();//���Win Socket����
    std::cout<<"���˳����������"<<std::endl;
    return 0;
}