//
// Created by ligang on 2020/7/22.
//

#include "SocketServer.h"

SocketServer::SocketServer(){
    _sock=INVALID_SOCKET;
    _recvCount=0;
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
    SOCKET cSock=INVALID_SOCKET;
#ifdef _WIN32
    cSock=accept(_sock,(sockaddr*)&clientAddr, &nAddrLen);
#else
    cSock=accept(_sock,(sockaddr*)&clientAddr, (socklen_t*)&nAddrLen);
#endif
    if(cSock==INVALID_SOCKET){
        std::cout<<"ERROR--> < socket= "<<cSock <<" > INVALID_SOCKET!"<<std::endl;
    }else{
        NewUserJoin userJoin((int)cSock);
        SendDataToAll(&userJoin);
        _clients.push_back(new ClientSocket(cSock));
//        std::cout<<"SUCCESS--> �¿ͻ���(count= "<<_clients.size() <<" ) ���룺IP = "<<inet_ntoa(clientAddr.sin_addr)<<" ,socket= "<<(int)cSock<<std::endl;
    }
    return cSock;
}


void SocketServer::Close() {
    if(_sock!=INVALID_SOCKET){
#ifdef _WIN32
        for(int n=(int)_clients.size()-1;n>=0;n--){
            closesocket(_clients[n]->sockfd());
            delete _clients[n];
        }
        ///8)�ر��׽���
        closesocket(_sock);
        ///
        WSACleanup();//���Win Socket����
#else
        for(int n=(int)_clients.size()-1;n>=0;n--){
            close(_clients[n]->sockfd());
            delete _clients[n];
        }
        ///8)�ر��׽���
        close(_sock);
#endif
        _clients.clear();
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
        for(int n=(int)_clients.size()-1;n>=0;n--){
            FD_SET(_clients[n]->sockfd(),&fdRead);
            if(maxSock<_clients[n]->sockfd()){
                maxSock=_clients[n]->sockfd();
            }
        }
        ///nfds��һ������ֵ����ָfd_set������������������socket���ķ�Χ������������
        ///�������ļ����������ֵ+1����windows�������������ν������д0
        timeval t={1,0};//������ģʽ
        int ret=select(maxSock+1,&fdRead,&fdWrite,&fdExp, &t);
//        std::cout<<"select ret= "<<ret<<"count= "<<_nCount++ <<std::endl;
        if(ret<0){
            std::cout<<"select�������~~~~~"<<std::endl;
            Close();
            return false;
        }
        if(FD_ISSET(_sock,&fdRead)) {
            FD_CLR(_sock, &fdRead);
            Accept();
            return true;
        }
        ///5�����տͻ�������
        for(int n=(int)_clients.size()-1;n>=0;n--){
            if(FD_ISSET(_clients[n]->sockfd(),&fdRead)){
                if(-1==RecvData(_clients[n])){
                    auto iter=_clients.begin()+n;
                    if(iter!=_clients.end()){
                        delete _clients[n];
                        _clients.erase(iter);
                    }
                }
            }
        }

        return true;
    }
    return false;
}

bool SocketServer::isRun() {
    return _sock!=INVALID_SOCKET;
}

int SocketServer::RecvData(ClientSocket* pClient){
//    char szRecv[4096]={};//���ջ�����
    int nLen=(int)recv(pClient->sockfd(),(char*)&_szRecv, RECV_BUFF_SIZE,0);//�����Ƚ��հ�ͷ��С
//    std::cout<<"Server nLen= "<<nLen<<std::endl;
    if(nLen<=0){
        std::cout<<" �ͻ���: "<<pClient->sockfd()<<" �˳�~~~~~"<<std::endl;
        return -1;
    }

    memcpy(pClient->msgBuf()+pClient->getLastPos(),_szRecv,nLen); //�����ݴӽ��ջ������������ڶ�������
    pClient->setLastPos(pClient->getLastPos()+nLen); //��Ϣ������������β��λ�ú���
    //�ж���Ϣ�����������ݳ��ȴ�����Ϣͷ����
    while (pClient->getLastPos() >= sizeof(DataHeader)){
        //��ʱ�Ϳ���֪����ǰ��Ϣ��ĳ���
        DataHeader* header=(DataHeader*)pClient->msgBuf();
        //�ж���Ϣ�����������ݳ��ȴ�����Ϣ����
        if(pClient->getLastPos() >= header->dataLength){
            //ʣ��δ������Ϣ���������ݵĳ���
            int nSize=pClient->getLastPos()-header->dataLength;
            //����������Ϣ
            OnNetMsg(pClient->sockfd(),header);
            //��δ��������ǰ��
            memcpy(pClient->msgBuf(),pClient->msgBuf()+header->dataLength,nSize);
            //��Ϣ������������β��λ��ǰ��
            pClient->setLastPos(nSize);
        }else{
            //ʣ�����ݳ���С��һ��������Ϣ����
            break;
        }
    }

    return 0;
}

void SocketServer::OnNetMsg(SOCKET cSock,DataHeader *header) {
    _recvCount++;
    auto t1=_tTime.getElapsedSecond();
    if(t1>=1.0){
        std::cout<<"time=< "<<t1<<" >, socket=< "<<_sock<<" >, recvCount=< "<<_recvCount<<" >"<<std::endl;
        _recvCount=0;
        _tTime.update();
    }
    ///6����������
    switch (header->cmd){
        case CMD_LOGIN:
        {
//            Login* login=(Login*)header;
//            std::cout<<"socket: "<<cSock<<" �յ����CMD_LOGIN ,���ݳ��ȣ�"<<login->dataLength
//                     <<" ,username: "<<login->userName<<" ,password: "<<login->PassWord<<std::endl;
            //�����ж��û������Ƿ���ȷ�Ĺ���
//            LoginResult ret;
//            SendData(cSock,&ret);
        }
            break;
        case CMD_LOGOUT:
        {
//            Logout* logout=(Logout*)header;
//            std::cout<<"socket: "<<cSock<<" �յ����CMD_LOGIN ,���ݳ��ȣ�"<<logout->dataLength
//                     <<" ,username: "<<logout->userName<<std::endl;
            //�����ж��û������Ƿ���ȷ�Ĺ���
//            LogoutResult ret;
//            SendData(cSock,&ret);
        }
            break;
        case CMD_ERROR:
        {
//            std::cout<<"socket= "<<(int)_sock<<" �յ��������Ϣ��CMD_ERROR,���ݳ��ȣ�"<<header->dataLength<<std::endl;
        }
            break;
        default:
        {
            std::cout<<"socket= "<<(int)_sock<<" �յ�δ������Ϣ,���ݳ��ȣ�"<<header->dataLength<<std::endl;
//            DataHeader header;
//            SendData(cSock,&header);
        }
            break;
    }
}

int SocketServer::SendData(SOCKET cSock, DataHeader *header) {
    if(isRun()&&header){
        return send(cSock,(const char*)header, header->dataLength,0);
    }
    return SOCKET_ERROR;
}

void SocketServer::SendDataToAll(DataHeader *header) {
    if(isRun()&&header){
        for(int n=(int)_clients.size()-1;n>=0;n--){
            SendData(_clients[n]->sockfd(), header);
        }
    }
}
