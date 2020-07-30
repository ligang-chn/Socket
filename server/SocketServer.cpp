//
// Created by ligang on 2020/7/22.
//

#include <functional>
#include "SocketServer.h"

CellServer::CellServer(SOCKET sock){
    _sock=sock;
    _pINetEvent= nullptr;
}

CellServer::~CellServer(){
    Close();
    _sock=INVALID_SOCKET;
}

void CellServer::setEventObj(INetEvent *event) {
    _pINetEvent=event;
}

void CellServer::Close() {
    if(_sock!=INVALID_SOCKET){
#ifdef _WIN32
        for(int n=(int)_clients.size()-1;n>=0;n--){
            closesocket(_clients[n]->sockfd());
            delete _clients[n];
        }
        ///8)关闭套接字
        closesocket(_sock);
#else
        for(int n=(int)_clients.size()-1;n>=0;n--){
            close(_clients[n]->sockfd());
            delete _clients[n];
        }
        ///8)关闭套接字
        close(_sock);
#endif
        _clients.clear();
    }
}


bool CellServer::isRun() {
    return _sock!=INVALID_SOCKET;
}

bool CellServer::OnRun(){
    while (isRun()){
        if(_clientsBuf.size()>0){//从缓冲队列里取出客户数据
            std::lock_guard<std::mutex> lock(_mutex);
            for(auto it:_clientsBuf){
                _clients.push_back(it);//将缓冲-->正式队列
            }
            _clientsBuf.clear();
        }

        //如果没有需要处理的客户端，就跳过
        if(_clients.empty()){
            std::chrono::milliseconds t(1);
            std::this_thread::sleep_for(t);//休眠1ms
            continue;
        }

        fd_set fdRead;
        FD_ZERO(&fdRead);

        SOCKET maxSock=_clients[0]->sockfd();//计算最大描述符
        for(int n=(int)_clients.size()-1;n>=0;n--){
            ///在CPU性能分析中，消耗占用45.2%资源，客户端数量4000规模
            ///这个地方可以优化：
            /// 1）如果没有新客户端加入，那么没有必要每次将客户端插入fdRead数组，
            ///     我们可以使用备份，在没有新客户端加入时，那么整理fdRead集合时就直接拷贝一下。
            ///2）使用多线程进行平摊这一部分的消耗，我们现在的消耗不在收数据那一块。
            FD_SET(_clients[n]->sockfd(),&fdRead);
            if(maxSock<_clients[n]->sockfd()){
                maxSock=_clients[n]->sockfd(); //查找最大的socket值
            }
        }
        ///nfds是一个整数值，是指fd_set集合中所有描述符（socket）的范围，而不是数量
        ///即所有文件描述符最大值+1，在windows中这个参数无所谓，可以写0
//        timeval t={1,0};//非阻塞模式
        ///在CPU性能分析中，消耗占用20.6%资源，客户端数量4000规模
        int ret=select(maxSock+1,&fdRead,nullptr,nullptr, nullptr);
        if(ret<0){
            std::cout<<"select任务结束~~~~~"<<std::endl;
            Close();
            return false;
        }

        ///5）接收客户端数据
        for(int n=(int)_clients.size()-1;n>=0;n--){
            ///在CPU性能分析中，消耗占用22.1%资源，客户端数量4000规模
            if(FD_ISSET(_clients[n]->sockfd(),&fdRead)){
                if(-1==RecvData(_clients[n])){///消耗11.4%资源
                    auto iter=_clients.begin()+n;
                    if(iter!=_clients.end()){
                        if(_pINetEvent)
                            _pINetEvent->OnLeave(_clients[n]);
                        delete _clients[n];
                        _clients.erase(iter);
                    }
                }
            }
        }
    }
//    return false;
}

int CellServer::RecvData(ClientSocket *pClient) {
//    char szRecv[4096]={};//接收缓冲区
    int nLen=(int)recv(pClient->sockfd(),(char*)&_szRecv, RECV_BUFF_SIZE,0);//数据先接收包头大小
//    std::cout<<"Server nLen= "<<nLen<<std::endl;
    if(nLen<=0){
//        std::cout<<" 客户端: "<<pClient->sockfd()<<" 退出~~~~~"<<std::endl;
        return -1;
    }

    memcpy(pClient->msgBuf()+pClient->getLastPos(),_szRecv,nLen); //将数据从接收缓冲区拷贝到第二缓冲区
    pClient->setLastPos(pClient->getLastPos()+nLen); //消息缓冲区的数据尾部位置后移
    //判断消息缓冲区的数据长度大于消息头长度
    while (pClient->getLastPos() >= sizeof(DataHeader)){
        //这时就可以知道当前消息体的长度
        DataHeader* header=(DataHeader*)pClient->msgBuf();
        //判断消息缓冲区的数据长度大于消息长度
        if(pClient->getLastPos() >= header->dataLength){
            //剩余未处理消息缓冲区数据的长度
            int nSize=pClient->getLastPos()-header->dataLength;
            //处理网络消息
            OnNetMsg(pClient->sockfd(),header);
            //将未处理数剧前移
            memcpy(pClient->msgBuf(),pClient->msgBuf()+header->dataLength,nSize);
            //消息缓冲区的数据尾部位置前移
            pClient->setLastPos(nSize);
        }else{
            //剩余数据长度小于一条完整消息长度
            break;
        }
    }

    return 0;
}

void CellServer::OnNetMsg(SOCKET cSock, DataHeader *header) {
    _pINetEvent->OnNetMsg(cSock,header);

//    auto t1=_tTime.getElapsedSecond();
//    if(t1>=1.0){
//        std::cout<<"time=< "<<t1<<" >, socket=< "<<_sock<<" >, recvCount=< "<<_recvCount<<" >"<<std::endl;
//        _recvCount=0;
//        _tTime.update();
//    }
    ///6）处理请求
    switch (header->cmd){
        case CMD_LOGIN:
        {
//            Login* login=(Login*)header;
//            std::cout<<"socket: "<<cSock<<" 收到命令：CMD_LOGIN ,数据长度："<<login->dataLength
//                     <<" ,username: "<<login->userName<<" ,password: "<<login->PassWord<<std::endl;
            //忽略判断用户密码是否正确的过程
//            LoginResult ret;
//            SendData(cSock,&ret);
        }
            break;
        case CMD_LOGOUT:
        {
//            Logout* logout=(Logout*)header;
//            std::cout<<"socket: "<<cSock<<" 收到命令：CMD_LOGIN ,数据长度："<<logout->dataLength
//                     <<" ,username: "<<logout->userName<<std::endl;
            //忽略判断用户密码是否正确的过程
//            LogoutResult ret;
//            SendData(cSock,&ret);
        }
            break;
        case CMD_ERROR:
        {
//            std::cout<<"socket= "<<(int)_sock<<" 收到服务端消息：CMD_ERROR,数据长度："<<header->dataLength<<std::endl;
        }
            break;
        default:
        {
            std::cout<<"socket= "<<(int)_sock<<" 收到未定义消息,数据长度："<<header->dataLength<<std::endl;
//            DataHeader header;
//            SendData(cSock,&header);
        }
            break;
    }
}

void CellServer::addClient(ClientSocket *pClient) {
    std::lock_guard<std::mutex> lock(_mutex);//使用自解锁
//    _mutex.lock();
    _clientsBuf.push_back(pClient);
//    _mutex.unlock();

}

void CellServer::Start() {
    _thread=std::thread(std::mem_fn(&CellServer::OnRun),this);
    //mem_fn 把成员函数转为函数对象，使用对象指针或对象（引用）进行绑定

}

size_t CellServer::getClientCount() {
    return _clients.size()+_clientsBuf.size();
}

///====================================================================================
SocketServer::SocketServer(){
    _sock=INVALID_SOCKET;
    _recvCount=0;
    _clientCount=0;
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
    SOCKET cSock=INVALID_SOCKET;
#ifdef _WIN32
    cSock=accept(_sock,(sockaddr*)&clientAddr, &nAddrLen);
#else
    cSock=accept(_sock,(sockaddr*)&clientAddr, (socklen_t*)&nAddrLen);
#endif
    if(cSock==INVALID_SOCKET){
        std::cout<<"ERROR--> < socket= "<<cSock <<" > INVALID_SOCKET!"<<std::endl;
    }else{
//        NewUserJoin userJoin((int)cSock);
//        SendDataToAll(&userJoin);
        //将新客户端放入消息服务线程的缓冲队列
        addClientToServer(new ClientSocket(cSock));
        _clientCount++;
//        std::cout<<"SUCCESS--> 新客户端(count= "<<_clients.size() <<" ) 加入：IP = "<<inet_ntoa(clientAddr.sin_addr)<<" ,socket= "<<(int)cSock<<std::endl;
    }
    return cSock;
}

void SocketServer::addClientToServer(ClientSocket *pClient) {
    //查找客户端数量最少的CellServer消息处理对象
    auto pMinServer=_cellServers[0];
    for(auto pCellServer:_cellServers){
        if(pMinServer->getClientCount()>pCellServer->getClientCount()){
            pMinServer=pCellServer;
        }
    }

    pMinServer->addClient(pClient);
}


void SocketServer::Close() {
    if(_sock!=INVALID_SOCKET){
#ifdef _WIN32
        ///8)关闭套接字
        closesocket(_sock);
        ///
        WSACleanup();//清除Win Socket环境
#else
        ///8)关闭套接字
        close(_sock);
#endif
    }
}

bool SocketServer::OnRun() {
    if(isRun()){
        time4msg();

        fd_set fdRead;
//        fd_set fdWrite;
//        fd_set fdExp;

        FD_ZERO(&fdRead);
//        FD_ZERO(&fdWrite);
//        FD_ZERO(&fdExp);

        FD_SET(_sock,&fdRead);//仅仅去读监听套接字，查看有没有客户端连接
//        FD_SET(_sock,&fdWrite);
//        FD_SET(_sock,&fdExp);

//        SOCKET maxSock=_sock;//计算最大描述符
//        for(int n=(int)_clients.size()-1;n>=0;n--){
//            ///在CPU性能分析中，消耗占用45.2%资源，客户端数量4000规模
//            ///这个地方可以优化：
//            /// 1）如果没有新客户端加入，那么没有必要每次将客户端插入fdRead数组，
//            ///     我们可以使用备份，在没有新客户端加入时，那么整理fdRead集合时就直接拷贝一下。
//            ///2）使用多线程进行平摊这一部分的消耗，我们现在的消耗不在收数据那一块。
//            FD_SET(_clients[n]->sockfd(),&fdRead);
//            if(maxSock<_clients[n]->sockfd()){
//                maxSock=_clients[n]->sockfd();
//            }
//        }
        ///nfds是一个整数值，是指fd_set集合中所有描述符（socket）的范围，而不是数量
        ///即所有文件描述符最大值+1，在windows中这个参数无所谓，可以写0
        timeval t={0,10};//非阻塞模式
        ///在CPU性能分析中，消耗占用20.6%资源，客户端数量4000规模
//        int ret=select(_sock+1,&fdRead,&fdWrite,&fdExp, &t);
        int ret=select(_sock+1,&fdRead, nullptr, nullptr, &t);
//        std::cout<<"select ret= "<<ret<<"count= "<<_nCount++ <<std::endl;
        if(ret<0){
            std::cout<<"select任务结束~~~~~"<<std::endl;
            Close();
            return false;
        }
        if(FD_ISSET(_sock,&fdRead)) {
            FD_CLR(_sock, &fdRead);
            Accept();
            return true;
        }
        ///5）接收客户端数据
//        for(int n=(int)_clients.size()-1;n>=0;n--){
//            ///在CPU性能分析中，消耗占用22.1%资源，客户端数量4000规模
//            if(FD_ISSET(_clients[n]->sockfd(),&fdRead)){
//                if(-1==RecvData(_clients[n])){///消耗11.4%资源
//                    auto iter=_clients.begin()+n;
//                    if(iter!=_clients.end()){
//                        delete _clients[n];
//                        _clients.erase(iter);
//                    }
//                }
//            }
//        }

        return true;
    }
    return false;
}

bool SocketServer::isRun() {
    return _sock!=INVALID_SOCKET;
}

int SocketServer::RecvData(ClientSocket* pClient){
//    char szRecv[4096]={};//接收缓冲区
    int nLen=(int)recv(pClient->sockfd(),(char*)&_szRecv, RECV_BUFF_SIZE,0);//数据先接收包头大小
//    std::cout<<"Server nLen= "<<nLen<<std::endl;
    if(nLen<=0){
//        std::cout<<" 客户端: "<<pClient->sockfd()<<" 退出~~~~~"<<std::endl;
        return -1;
    }

    memcpy(pClient->msgBuf()+pClient->getLastPos(),_szRecv,nLen); //将数据从接收缓冲区拷贝到第二缓冲区
    pClient->setLastPos(pClient->getLastPos()+nLen); //消息缓冲区的数据尾部位置后移
    //判断消息缓冲区的数据长度大于消息头长度
    while (pClient->getLastPos() >= sizeof(DataHeader)){
        //这时就可以知道当前消息体的长度
        DataHeader* header=(DataHeader*)pClient->msgBuf();
        //判断消息缓冲区的数据长度大于消息长度
        if(pClient->getLastPos() >= header->dataLength){
            //剩余未处理消息缓冲区数据的长度
            int nSize=pClient->getLastPos()-header->dataLength;
            //处理网络消息
//            OnNetMsg(pClient->sockfd(),header);
            //将未处理数剧前移
            memcpy(pClient->msgBuf(),pClient->msgBuf()+header->dataLength,nSize);
            //消息缓冲区的数据尾部位置前移
            pClient->setLastPos(nSize);
        }else{
            //剩余数据长度小于一条完整消息长度
            break;
        }
    }

    return 0;
}

//计算并输出每秒收到的网络消息
void SocketServer::time4msg() {
    auto t1=_tTime.getElapsedSecond();
    if(t1>=1.0){
        std::cout<<"thread=< "<<_cellServers.size()<<" >, "<<"time=< "<<t1<<" >, socket=< "<<_sock<<" >, clients=< "<<(int)_clientCount<<" >, recvCount=< "<<(int)(_recvCount/t1)<<" >"<<std::endl;
        _recvCount=0;
        _tTime.update();
    }
}


int SocketServer::SendData(SOCKET cSock, DataHeader *header) {
    if(isRun()&&header){
        return send(cSock,(const char*)header, header->dataLength,0);
    }
    return SOCKET_ERROR;
}

void SocketServer::Start() {
    for(int n=0;n<_CellServer_THREAD_COUNT;n++){
        auto ser=new CellServer(_sock);
        _cellServers.push_back(ser);
        //注册网络事件接受对象
        ser->setEventObj(this);
        //启动消息处理线程
        ser->Start();
    }
}

void SocketServer::OnLeave(ClientSocket *pClient) {
    _clientCount--;
}

void SocketServer::OnNetMsg(SOCKET _cSock, DataHeader *header) {
//    time4msg();
    _recvCount++;
}