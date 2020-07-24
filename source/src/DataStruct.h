//
// Created by ligang on 2020/6/24.
//

#ifndef SOCKET_DATASTRUCT_H
#define SOCKET_DATASTRUCT_H


enum CMD{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
    CMD_NEW_USER_JOIN,
    CMD_ERROR
};

//网络报文数据定义
struct DataHeader{
    short cmd;
    short dataLength;
};
//DataPackage
struct Login: public DataHeader{
    Login(){
        dataLength= sizeof(Login);
        cmd=CMD_LOGIN;
    }
    char userName[32];
    char PassWord[32];
};

struct LoginResult:public DataHeader{
    LoginResult(){
        dataLength= sizeof(LoginResult);
        cmd=CMD_LOGIN_RESULT;
        result=0;
    }
    int result;
    char data[1024];
};

struct Logout:public DataHeader{
    Logout(){
        dataLength= sizeof(Logout);
        cmd=CMD_LOGOUT;
    }
    char userName[32];
};

struct LogoutResult:public DataHeader{
    LogoutResult(){
        dataLength= sizeof(LogoutResult);
        cmd=CMD_LOGOUT_RESULT;
        result=0;
    }
    int result;
//    char PassWord[32];
};

struct NewUserJoin:public DataHeader{
    NewUserJoin(int msock){
        dataLength= sizeof(NewUserJoin);
        cmd=CMD_NEW_USER_JOIN;
        sock=msock;
    }
    int sock;
//    char PassWord[32];
};

#endif //SOCKET_DATASTRUCT_H
