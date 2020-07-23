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
    WORD ver=MAKEWORD(2,2);//版本号
    WSADATA dat;
    WSAStartup(ver,&dat);//启动windows的socket 2.x网络环境
#endif
    if(_sock!=INVALID_SOCKET){
        std::cout<<"< Socket = "<< _sock <<" > existed, Now stopping old socket!#########"<<std::endl;
        Close();
    }
    ///1）建立一个socket套接字——这是监听套接字
    _sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//  ipv4网络，流类型，tcp/udp
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
    ///2)绑定端口
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
    ///3)监听端口
    if(SOCKET_ERROR== ret)
    {
        std::cout<<"ERROR-->listen < Socket= "<<_sock <<" > failed!"<<std::endl;
    }else{
        std::cout<<"SUCCESS-->listen < Socket= "<<_sock <<" > success!"<<std::endl;
    }
    return ret;
}

SOCKET SocketServer::Accept() {
    ///4)accept 等待接收客户端连接
    sockaddr_in clientAddr={};//存放返回的客户端地址
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
        std::cout<<"SUCCESS--> 新客户端加入：IP = "<<inet_ntoa(clientAddr.sin_addr)<<" ,socket= "<<(int)_cSock<<std::endl;
    }
    return _cSock;
}


void SocketServer::Close() {
    if(_sock!=INVALID_SOCKET){
#ifdef _WIN32
        for(int n=(int)g_clients.size()-1;n>=0;n--){
            closesocket(g_clients[n]);
        }
        ///8)关闭套接字
        closesocket(_sock);
        ///
        WSACleanup();//清除Win Socket环境
#else
        for(int n=(int)g_clients.size()-1;n>=0;n--){
        close(g_clients[n]);
    }
    ///8)关闭套接字
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

        SOCKET maxSock=_sock;//计算最大描述符
        for(int n=(int)g_clients.size()-1;n>=0;n--){
            FD_SET(g_clients[n],&fdRead);
            if(maxSock<g_clients[n]){
                maxSock=g_clients[n];
            }
        }
        ///nfds是一个整数值，是指fd_set集合中所有描述符（socket）的范围，而不是数量
        ///即所有文件描述符最大值+1，在windows中这个参数无所谓，可以写0
        timeval t={1,0};//非阻塞模式
        int ret=select(maxSock+1,&fdRead,&fdWrite,&fdExp, &t);
        if(ret<0){
            std::cout<<"select任务结束~~~~~"<<std::endl;
            Close();
            return false;
        }
        if(FD_ISSET(_sock,&fdRead)) {
            FD_CLR(_sock, &fdRead);
            Accept();
        }
        ///5）接收客户端数据
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
    char szRecv[1024]={};//接收缓冲区
    int nLen=(int)recv(_cSock,(char*)&szRecv, sizeof(DataHeader),0);//数据先接收包头大小
    DataHeader* header=(DataHeader*)szRecv;
    if(nLen<=0){
        std::cout<<" 客户端: "<<_cSock<<" 退出~~~~~"<<std::endl;
        return -1;
    }
    recv(_cSock,szRecv+ sizeof(DataHeader), header->dataLength- sizeof(DataHeader),0);
    OnNetMsg(_cSock,header);
    return 0;
}

void SocketServer::OnNetMsg(SOCKET _cSock,DataHeader *header) {
    ///6）处理请求
    switch (header->cmd){
        case CMD_LOGIN:
        {
            Login* login=(Login*)header;
            std::cout<<"socket: "<<_cSock<<" 收到命令：CMD_LOGIN ,数据长度："<<login->dataLength
                     <<" ,username: "<<login->userName<<" ,password: "<<login->PassWord<<std::endl;
            //忽略判断用户密码是否正确的过程
            LoginResult ret;
            send(_cSock,(char*)&ret, sizeof(LoginResult),0);
        }
            break;
        case CMD_LOGOUT:
        {
            Logout* logout=(Logout*)header;
            std::cout<<"socket: "<<_cSock<<" 收到命令：CMD_LOGIN ,数据长度："<<logout->dataLength
                     <<" ,username: "<<logout->userName<<std::endl;
            //忽略判断用户密码是否正确的过程
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
