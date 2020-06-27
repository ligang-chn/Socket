//
// Created by ligang on 2020/6/26.
//

#include "SocketClient.h"


SocketClient::SocketClient(){
    _sock=INVALID_SOCKET;
}

SocketClient::~SocketClient(){
    Close();
}


void SocketClient::InitSocket() {
#ifdef _WIN32
    WORD ver=MAKEWORD(2,2);//�汾��
    WSADATA dat;
    WSAStartup(ver,&dat);//����windows��socket 2.x���绷��
#endif
    ///1������һ��socket�׽���
    if(_sock!=INVALID_SOCKET){
        std::cout<<"socket: "<< _sock <<" existed, Now stopping old socket!"<<std::endl;
        Close();
    }
    _sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//  ipv4���磬�����ͣ�tcp/udp
    if(INVALID_SOCKET==_sock){
        std::cout<<"ERROR-->build socket failed!"<<std::endl;
    }else{
        std::cout<<"OK-->build socket: ( "<< (int)_sock <<" ) success!"<<std::endl;
    }
}

int SocketClient::Connect(const char *ip, unsigned short port) {
    if(_sock==INVALID_SOCKET){
        InitSocket();
    }
    ///2)connect���ӷ�����
    sockaddr_in _sin={};
    _sin.sin_family=AF_INET;
    _sin.sin_port=htons(port);//host to net unsigned short

#ifdef  _WIN32
    _sin.sin_addr.S_un.S_addr= inet_addr(ip);
#else
    _sin.sin_addr.s_addr= inet_addr(ip);
#endif
    int ret=connect(_sock,(sockaddr*)&_sin, sizeof(sockaddr_in));
    if(SOCKET_ERROR==ret){
        std::cout<<"ERROR-->socket= "<<(int)_sock<<" connect server: ( ip= "
        <<ip<<" , port= "<<port<<" ) failed!"<<std::endl;
    }else{
        std::cout<<"success-->socket= "<<(int)_sock<<" connect server: ( ip= "
        <<ip<<" , port= "<<port<<" ) success!"<<std::endl;
    }
    return ret;
}

void SocketClient::Close() {
    if(_sock!=INVALID_SOCKET){
        ///7)�ر��׽���
#ifdef  _WIN32
        closesocket(_sock);
        ///
        WSACleanup();//���Win Socket����
#else
        close(_sock);
#endif
        _sock=INVALID_SOCKET;
    }
}


bool SocketClient::OnRun() {
    if(isRun()){
        fd_set fdReads;
        FD_ZERO(&fdReads);
        FD_SET(_sock,&fdReads);
        timeval t={0,0};
        int ret=select(_sock+1,&fdReads,0,0,&t);
        if(ret<0){
            std::cout<<"socket= "<<(int)_sock<<" select�������"<<std::endl;
            Close();
            return false;
        }
        if(FD_ISSET(_sock,&fdReads)){
            FD_CLR(_sock,&fdReads);

            if(-1==RecvData(_sock)){
                std::cout<<"socket= "<<(int)_sock<<" select�������2"<<std::endl;
                Close();
                return false;
            }
        }
        return true;
    }
    return false;
}

bool SocketClient::isRun() {
    return _sock!=INVALID_SOCKET;
}

//�������� ����ճ�����ְ�
int SocketClient::RecvData(SOCKET _cSock){
    char szRecv[1024]={};//���ջ�����
    int nLen=recv(_cSock,(char*)&szRecv, sizeof(DataHeader),0);//�����Ƚ��հ�ͷ��С
    DataHeader* header=(DataHeader*)szRecv;
    if(nLen<=0){
        std::cout<<"socket= "<<(int)_cSock<<" ��������Ͽ�����~~~~~"<<std::endl;
        return -1;
    }
    recv(_cSock,szRecv+ sizeof(DataHeader), header->dataLength- sizeof(DataHeader),0);
    OnNetMsg(header);
    return 0;
}

void SocketClient::OnNetMsg(DataHeader* header){
    ///6����������
    switch (header->cmd){
        case CMD_LOGIN_RESULT:
        {
            LoginResult* loginResult=(LoginResult*)header;
            std::cout<<"socket= "<<(int)_sock<<" �յ��������Ϣ��CMD_LOGIN_RESULT,���ݳ��ȣ�"<<loginResult->dataLength
                     <<" ,����: "<<loginResult->result<<std::endl;
        }
            break;
        case CMD_LOGOUT_RESULT:
        {
             LogoutResult* logoutResult=(LogoutResult*)header;
            std::cout<<"socket= "<<(int)_sock<<" �յ��������Ϣ��CMD_LOGOUT_RESULT,���ݳ��ȣ�"<<logoutResult->dataLength
                     <<" ,����: "<<logoutResult->result<<std::endl;
        }
            break;
        case CMD_NEW_USER_JOIN:
        {
            NewUserJoin* userJoin=(NewUserJoin*)header;
            std::cout<<"socket= "<<(int)_sock<<" �յ��������Ϣ��CMD_NEW_USER_JOIN,���ݳ��ȣ�"<<userJoin->dataLength
                     <<" ,����: "<<userJoin->sock<<std::endl;
        }
            break;
        default:
            break;
    }
}

int SocketClient::SendData(DataHeader *header) {
    if(isRun()&&header){
        return send(_sock,(const char*)header, header->dataLength,0);
    }
    return SOCKET_ERROR;
}