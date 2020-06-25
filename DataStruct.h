//
// Created by ligang on 2020/6/24.
//

#ifndef SOCKET_DATASTRUCT_H
#define SOCKET_DATASTRUCT_H


enum CMD{
    CMD_LOGIN,
    CMD_LOGOUT,
    CMD_ERROR
};

//网络报文数据定义
struct DataHeader{
    short cmd;
    short dataLength;
};
//DataPackage
struct Login{
    char userName[32];
    char PassWord[32];
};

struct LoginResult{
    int result;
//    char PassWord[32];
};

struct Logout{
    char userName[32];
};

struct LogoutResult{
    int result;
//    char PassWord[32];
};

#endif //SOCKET_DATASTRUCT_H
