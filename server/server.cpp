//
// Created by ligang on 2020/6/23.
//
#if 1

#include <iostream>
#include "SocketServer.h"

using namespace std;

bool g_bRun=true;
///���̴߳�����
void cmdThread(){
    while (true){   //����߳�һֱ������scanf�����ȴ���������
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

int main(){

    SocketServer server;
    server.InitSocket();
    server.Bind(nullptr,9999);
    server.Listen(5);
    server.Start();
//    SocketServer server1;
//    server1.InitSocket();
//    server1.Bind(nullptr,9998);
//    server1.Listen(5);
    ///�����߳�
    std::thread t1(cmdThread);
    t1.detach();//Detach �̡߳� ����ǰ�̶߳����������ִ��ʵ������̶߳�����룬ʹ���̵߳�ִ�п��Ե������С�
    // һ���߳�ִ����ϣ������������Դ���ᱻ�ͷš�
    //�������˼���ǣ�ʹ��detach,main�������õȴ��߳̽������ܽ�������ʱ���̻߳�û�н�����main�������Ѿ������ˡ�

    while (g_bRun){ //||server1.isRun()
        server.OnRun();   //�ȴ��ͻ������ӡ����������߳�
//        server1.OnRun();
    }
    server.Close();
//    server1.Close();
    cout<<"���˳�"<<endl;
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
#include <vector>
#include <algorithm>

std::vector<SOCKET> g_clients;

int processor(SOCKET _cSock){
    char szRecv[1024]={};//���ջ�����
    int nLen=(int)recv(_cSock,(char*)&szRecv, sizeof(DataHeader),0);//�����Ƚ��հ�ͷ��С
    DataHeader* header=(DataHeader*)szRecv;
    if(nLen<=0){
        std::cout<<_cSock<<" �ͻ����˳�~~~~~"<<std::endl;
        return -1;
    }

    ///6����������
    switch (header->cmd){
        case CMD_LOGIN:
        {
            recv(_cSock,szRecv+ sizeof(DataHeader), header->dataLength- sizeof(DataHeader),0);
            Login* login=(Login*)szRecv;
            std::cout<<"socket: "<<_cSock<<" �յ����CMD_LOGIN ,���ݳ��ȣ�"<<login->dataLength
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
            std::cout<<"socket: "<<_cSock<<" �յ����CMD_LOGIN ,���ݳ��ȣ�"<<logout->dataLength
                     <<" ,username: "<<logout->userName<<std::endl;
            //�����ж��û������Ƿ���ȷ�Ĺ���
            LogoutResult ret;
            send(_cSock,(char*)&ret, sizeof(LogoutResult),0);
        }
            break;
        default:
        {
            DataHeader header={CMD_ERROR,0};
            send(_cSock,(char*)&header, sizeof(DataHeader),0);
        }

            break;
    }
    return 0;
}

int main() {
#ifdef _WIN32
    WORD ver=MAKEWORD(2,2);//�汾��
    WSADATA dat;
    WSAStartup(ver,&dat);//����windows��socket 2.x���绷��
#endif

    ///1������һ��socket�׽���
    SOCKET _sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//  ipv4���磬�����ͣ�tcp/udp
    sockaddr_in _sin={};
    _sin.sin_family=AF_INET;
    _sin.sin_port=htons(9999);//host to net unsigned short
#ifdef _WIN32
    _sin.sin_addr.S_un.S_addr= INADDR_ANY; //inet_addr("127.0.0.1");
#else
    _sin.sin_addr.s_addr= INADDR_ANY;
#endif
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


    while (true){
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExp;

        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExp);

        FD_SET(_sock,&fdRead);
        FD_SET(_sock,&fdWrite);
        FD_SET(_sock,&fdExp);

        SOCKET maxSock=_sock;//�������������
        for(int n=(int)g_clients.size()-1;n>=0;n--){
            FD_SET(g_clients[n],&fdRead);
            if(maxSock<g_clients[n]){
                maxSock=g_clients[n];
            }
        }
        ///nfds��һ������ֵ����ָfd_set������������������socket���ķ�Χ������������
        ///�������ļ����������ֵ+1����windows�������������ν������д0
        timeval t={1,0};//������ģʽ
        int ret=select(maxSock+1,&fdRead,&fdWrite,&fdExp, &t);
        if(ret<0){
            std::cout<<"select�������~~~~~"<<std::endl;
            break;
        }
        if(FD_ISSET(_sock,&fdRead)){
            FD_CLR(_sock,&fdRead);
            ///4)accept �ȴ����տͻ�������
            sockaddr_in clientAddr={};//��ŷ��صĿͻ��˵�ַ
            int nAddrLen= sizeof(sockaddr_in);
            SOCKET _cSock=INVALID_SOCKET;
#ifdef _WIN32
            _cSock=accept(_sock,(sockaddr*)&clientAddr, &nAddrLen);
#else
            _cSock=accept(_sock,(sockaddr*)&clientAddr, (socklen_t*)&nAddrLen);
#endif
            if(_cSock==INVALID_SOCKET){
                std::cout<<"ERROR-->INVALID_SOCKET!"<<std::endl;
            }else{
                for(int n=(int)g_clients.size()-1;n>=0;n--){
                    NewUserJoin userJoin((int)_cSock);
                    send(g_clients[n],(const char*)&userJoin, sizeof(NewUserJoin),0);
                }
                g_clients.push_back(_cSock);
                std::cout<<"�¿ͻ��˼��룺IP = "<<inet_ntoa(clientAddr.sin_addr)<<" ,Port= "<<(int)_cSock<<std::endl;
            }
        }


        ///5�����տͻ�������
#ifdef _WIN32
        for(int n=0;n<fdRead.fd_count;n++){
            if(-1==processor(fdRead.fd_array[n])){
                auto iter=std::find(g_clients.begin(),g_clients.end(),fdRead.fd_array[n]);
                if(iter!=g_clients.end()){
                    g_clients.erase(iter);
                }
            }
        }
#else
        for(int n=(int)g_clients.size()-1;n>=0;n--){
            if(FD_ISSET(g_clients[n],&fdRead)){
                if(-1==processor(g_clients[n])){
                    auto iter=g_clients.begin()+n;
                    if(iter!=g_clients.end()){
                        g_clients.erase(iter);
                    }
                }
            }
        }
#endif
        // std::cout<<"ִ����������..."<<std::endl;

    }
#ifdef _WIN32
    for(size_t n=g_clients.size()-1;n>=0;n--){
        closesocket(g_clients[n]);
    }
    ///8)�ر��׽���
    closesocket(_sock);
    ///
    WSACleanup();//���Win Socket����
#else
    for(int n=(int)g_clients.size()-1;n>=0;n--){
        close(g_clients[n]);
    }
    ///8)�ر��׽���
    close(_sock);
#endif
    std::cout<<"���˳����������"<<std::endl;
    return 0;
}
#endif


