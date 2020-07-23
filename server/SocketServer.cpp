//
// Created by ligang on 2020/7/22.
//

#include "SocketServer.h"

SocketServer::SocketServer(){
    _sock=INVALID_SOCKET;
}

SocketServer::~SocketServer() {
    Close();
}

SOCKET SocketServer::InitSocket() {
#ifdef _WIN32
    WORD ver=MAKEWORD(2,2);//�汾��
    WSADATA dat;
    WSAStartup(ver,&dat);//����windows��socket 2.x���绷��
#endif
    if(_sock!=INVALID_SOCKET){
        std::cout<<"< Socket = "<< _sock <<" > existed, Now stopping old socket!#########"<<std::endl;
        Close();
    }
    ///1������һ��socket�׽��֡������Ǽ����׽���
    _sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//  ipv4���磬�����ͣ�tcp/udp
    if(INVALID_SOCKET==_sock){
        std::cout<<"ERROR-->build socket failed!"<<std::endl;
    }else{
        std::cout<<"SUCCESS-->build < Socket = "<< (int)_sock <<" > success!"<<std::endl;
    }
    return _sock;
}

int SocketServer::Bind(const char *ip, unsigned short port) {
//    if(INVALID_SOCKET == _sock){
//        InitSocket();
//    }
    sockaddr_in _sin={};
    _sin.sin_family=AF_INET;
    _sin.sin_port=htons(port);//host to net unsigned short

#ifdef _WIN32
    if(ip){
        _sin.sin_addr.S_un.S_addr= inet_addr(ip); //inet_addr("127.0.0.1");
    }else{
        _sin.sin_addr.S_un.S_addr= INADDR_ANY; //inet_addr("127.0.0.1");
    }
#else
    if(ip){
        _sin.sin_addr.s_addr= inet_addr(ip); //inet_addr("127.0.0.1");
    }else{
        _sin.sin_addr.s_addr= INADDR_ANY; //inet_addr("127.0.0.1");
    }
#endif
    ///2)�󶨶˿�
    int ret=bind(_sock,(sockaddr*)&_sin, sizeof(_sin));
    if(SOCKET_ERROR== ret)
    {
        std::cout<<"ERROR-->bind <Port = "<<port<<" > failed!"<<std::endl;
    }else{
        std::cout<<"SUCCESS-->bind <Port = "<<port<<" > success!"<<std::endl;
    }

    return ret;
}

int SocketServer::Listen(int n) {
    int ret=listen(_sock,n);
    ///3)�����˿�
    if(SOCKET_ERROR== ret)
    {
        std::cout<<"ERROR-->listen < Socket= "<<_sock <<" > failed!"<<std::endl;
    }else{
        std::cout<<"SUCCESS-->listen < Socket= "<<_sock <<" > success!"<<std::endl;
    }
    return ret;
}

SOCKET SocketServer::Accept() {
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
        std::cout<<"ERROR--> < socket= "<<_cSock <<" > INVALID_SOCKET!"<<std::endl;
    }else{
        NewUserJoin userJoin((int)_cSock);
        SendDataToAll(&userJoin);
        g_clients.push_back(_cSock);
        std::cout<<"SUCCESS--> �¿ͻ��˼��룺IP = "<<inet_ntoa(clientAddr.sin_addr)<<" ,socket= "<<(int)_cSock<<std::endl;
    }
    return _cSock;
}


void SocketServer::Close() {
    if(_sock!=INVALID_SOCKET){
#ifdef _WIN32
        for(int n=(int)g_clients.size()-1;n>=0;n--){
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
    }
}

bool SocketServer::OnRun() {
    if(isRun()){
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
            Close();
            return false;
        }
        if(FD_ISSET(_sock,&fdRead)) {
            FD_CLR(_sock, &fdRead);
            Accept();
        }
        ///5�����տͻ�������
#ifdef _WIN32
        for(int n=0;n<fdRead.fd_count;n++){
            if(-1==RecvData(fdRead.fd_array[n])){
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
        return true;
    }
    return false;
}

bool SocketServer::isRun() {
    return _sock!=INVALID_SOCKET;
}

int SocketServer::RecvData(SOCKET _cSock){
    char szRecv[1024]={};//���ջ�����
    int nLen=(int)recv(_cSock,(char*)&szRecv, sizeof(DataHeader),0);//�����Ƚ��հ�ͷ��С
    DataHeader* header=(DataHeader*)szRecv;
    if(nLen<=0){
        std::cout<<" �ͻ���: "<<_cSock<<" �˳�~~~~~"<<std::endl;
        return -1;
    }
    recv(_cSock,szRecv+ sizeof(DataHeader), header->dataLength- sizeof(DataHeader),0);
    OnNetMsg(_cSock,header);
    return 0;
}

void SocketServer::OnNetMsg(SOCKET _cSock,DataHeader *header) {
    ///6����������
    switch (header->cmd){
        case CMD_LOGIN:
        {
            Login* login=(Login*)header;
            std::cout<<"socket: "<<_cSock<<" �յ����CMD_LOGIN ,���ݳ��ȣ�"<<login->dataLength
                     <<" ,username: "<<login->userName<<" ,password: "<<login->PassWord<<std::endl;
            //�����ж��û������Ƿ���ȷ�Ĺ���
            LoginResult ret;
            send(_cSock,(char*)&ret, sizeof(LoginResult),0);
        }
            break;
        case CMD_LOGOUT:
        {
            Logout* logout=(Logout*)header;
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
}

int SocketServer::SendData(SOCKET _cSock, DataHeader *header) {
    if(isRun()&&header){
        return send(_cSock,(const char*)header, header->dataLength,0);
    }
    return SOCKET_ERROR;
}

void SocketServer::SendDataToAll(DataHeader *header) {
    if(isRun()&&header){
        for(int n=(int)g_clients.size()-1;n>=0;n--){
            SendData(g_clients[n], header);
        }
    }
}
