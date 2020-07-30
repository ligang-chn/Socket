//
// Created by ligang on 2020/6/23.
//
#if 1
#include <iostream>
#include <string.h>
#include "SocketClient.h"

bool g_bRun=true;

//�ͻ�������
const int cCount=10000;//FD_SETSIZE-1;
//�����߳�����
const int tCount=4;
//�ͻ�������
SocketClient* client[cCount];


///���̴߳�����
void cmdThread(){
    while (true){
        char cmdBuf[256]={};
        scanf("%s",cmdBuf);
        if(0==strcmp(cmdBuf,"exit")) {
            g_bRun=false;
            std::cout << "�˳�cmdThread�̣߳�" << std::endl;
            break;
//        }else if(0==strcmp(cmdBuf,"login")){
//            Login login;
//            strcpy(login.userName,"ligang");
//            strcpy(login.PassWord,"123456");
//            client->SendData(&login);
//        }else if(0==strcmp(cmdBuf,"logout")){
//            Logout logout;
//            strcpy(logout.userName,"ligang");
//            client->SendData(&logout);
        }else{
            std::cout<<"��֧�ָ����"<<std::endl;
        }
    }
}


void sendThread(int id){
    std::cout<<"thread= "<<id<<" start!"<<std::endl;
    //4���߳� ID 1~4
    int c=cCount/tCount;//����ÿ���߳���Ҫ���ӵĿͻ�����������1000/4=250
    int begin=(id-1)*c;//����ÿ���̵߳Ŀͻ�����ʼλ��
    int end=id*c;//�������λ��

    for(int i=begin;i<end;i++){
        client[i]=new SocketClient();//�����ͻ��˶���
    }

    for(int i=begin;i<end;i++){
        client[i]->Connect("127.0.0.1",9999);
//        client[i]->Connect("121.199.78.48",9999);
//        client[i]->Connect("192.168.181.146",9999);
//        std::cout<<"thread= "<<id<<" , Connect= "<<i<<std::endl;
    }
    std::cout<<"thread= "<<id<<" , Connect=< begin= "<<begin<<" ,end= "<<end<<" >"<<std::endl;
//    client.InitSocket();
//    client.Connect("127.0.0.1",9999);
//    client.Connect("121.199.78.48",9999);

//    SocketClient client2;
////    client.InitSocket();
//    client2.Connect("192.168.181.146",9999);
//    std::thread t2(cmdThread,&client2);
//    t2.detach();//Detach �̡߳� ����ǰ�̶߳����������ִ��ʵ������̶߳�����룬ʹ���̵߳�ִ�п��Ե������С�
//
//    SocketClient client3;
////    client.InitSocket();
//    client3.Connect("121.199.78.48",9999);
//    std::thread t3(cmdThread,&client3);
//    t3.detach();//Detach �̡߳� ����ǰ�̶߳����������ִ��ʵ������̶߳�����룬ʹ���̵߳�ִ�п��Ե������С�

    std::chrono::milliseconds t(3000);
    std::this_thread::sleep_for(t);

    Login login[10];
    for(int n=0;n<10;n++){
        strcpy(login[n].userName,"ligang");
        strcpy(login[n].PassWord,"123456");
    }
    const int nLen=sizeof(login);

    while (g_bRun ){//|| client2.isRun()||client3.isRun()
        for(int n=begin;n<end;n++){
            client[n]->SendData(login,nLen);
            client[n]->OnRun();
        }

//        client2.OnRun();
//        client3.OnRun();
    }

//    client3.Close();
//    client2.Close();
    for(int n=begin;n<end;n++){
        client[n]->Close();
        delete client[n];
    }
    std::cout<<"thread= "<<id<<" exit!"<<std::endl;
}

int main(){
    ///����UI�߳�
    std::thread t1(cmdThread);
    t1.detach();//Detach �̡߳� ����ǰ�̶߳����������ִ��ʵ������̶߳�����룬ʹ���̵߳�ִ�п��Ե������С�
    // һ���߳�ִ����ϣ������������Դ���ᱻ�ͷš�
    //�������˼���ǣ�ʹ��detach,main�������õȴ��߳̽������ܽ�������ʱ���̻߳�û�н�����main�������Ѿ������ˡ�

    //���������߳�
    for(int n=0;n<tCount;n++){
        std::thread t2(sendThread,n+1);
        t2.detach();//Detach �̡߳� ����ǰ�̶߳����������ִ��ʵ������̶߳�����룬ʹ���̵߳�ִ�п��Ե������С�
    }

    while(g_bRun){
        Sleep(100);
    }

    std::cout<<"���߳����˳����������"<<std::endl;

    return 0;
}
#endif

#if 0
#define  WIN32_LEAN_AND_MEAN //��Ҫ���WinSock2.hͷ�ļ���������

#ifdef _WIN32
    #include <windows.h>
    #include <WinSock2.h>
    #include <inaddr.h>//���������֮ǰclion�Զ������

    #include "../DataStruct.h"
#else
#include <unistd.h>//uni std
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <string.h>
    #include <stdlib.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include "./DataStruct.h"

    #define SOCKET int
    #define INVALID_SOCKET (SOCKET)(~0)
    #define SOCKET_ERROR           (-1)
#endif

#include <iostream>
#include <stdlib.h>
#include <thread>



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

bool g_bRun=true;
///���̴߳�����
void cmdThread(SOCKET sock){
    while (true){
        char cmdBuf[256]={};
        scanf("%s",cmdBuf);
        if(0==strcmp(cmdBuf,"exit")){
            g_bRun=false;
            std::cout<<"�˳�cmdThread�̣߳�"<<std::endl;
            break;
        }else if(0==strcmp(cmdBuf,"login")){
            Login login;
            strcpy(login.userName,"ligang");
            strcpy(login.PassWord,"123456");
            send(sock,(const char*)&login, sizeof(Login),0);
        }else if(0==strcmp(cmdBuf,"logout")){
            Logout logout;
            strcpy(logout.userName,"ligang");
            send(sock,(const char*)&logout, sizeof(Logout),0);
        }else{
            std::cout<<"��֧�ָ����"<<std::endl;
        }
    }
}


int main() {
#ifdef _WIN32
    WORD ver=MAKEWORD(2,2);//�汾��
    WSADATA dat;
    WSAStartup(ver,&dat);//����windows��socket 2.x���绷��
#endif
    ///1������һ��socket�׽���
    SOCKET _sock=socket(AF_INET,SOCK_STREAM,0);//  ipv4���磬�����ͣ�tcp/udp
    if(INVALID_SOCKET==_sock){
        std::cout<<"ERROR-->build socket failed!"<<std::endl;
    }
    ///2)connect���ӷ�����
    sockaddr_in _sin={};
    _sin.sin_family=AF_INET;
    _sin.sin_port=htons(9999);//host to net unsigned short

#ifdef  _WIN32
    _sin.sin_addr.S_un.S_addr= inet_addr("127.0.0.1");
#else
    _sin.sin_addr.s_addr= inet_addr("127.0.0.1");
#endif
    int ret=connect(_sock,(sockaddr*)&_sin, sizeof(sockaddr_in));
    if(SOCKET_ERROR==ret){
        std::cout<<"ERROR-->connect failed!"<<std::endl;
    }else{
        std::cout<<"success-->connect success!"<<std::endl;
    }

    ///�����߳�
    std::thread t1(cmdThread,_sock);
    t1.detach();//Detach �̡߳� ����ǰ�̶߳����������ִ��ʵ������̶߳�����룬ʹ���̵߳�ִ�п��Ե������С�
    // һ���߳�ִ����ϣ������������Դ���ᱻ�ͷš�
    //�������˼���ǣ�ʹ��detach,main�������õȴ��߳̽������ܽ�������ʱ���̻߳�û�н�����main�������Ѿ������ˡ�

    while(g_bRun){
        fd_set fdReads;
        FD_ZERO(&fdReads);
        FD_SET(_sock,&fdReads);
        timeval t={1,0};
        int ret=select(_sock+1,&fdReads,0,0,&t);
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

        ///�߳�thread
//        std::cout<<"ִ����������..."<<std::endl;
//        Sleep(1000);//1000ms
    }
    ///7)�ر��׽���
#ifdef  _WIN32
    closesocket(_sock);
    ///
    WSACleanup();//���Win Socket����
#else
    close(_sock);
#endif

    std::cout<<"���˳����������"<<std::endl;
    return 0;
}
#endif

