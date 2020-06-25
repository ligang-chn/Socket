#define  WIN32_LEAN_AND_MEAN //主要解决WinSock2.h头文件引入问题

#include <windows.h>
#include <WinSock2.h>
#include <iostream>
using namespace std;
struct s{
    double c;
    char b;
    int a;
};

s *sa;

struct stu
 {
     union
     {
         char b;
         double x;
     }uu;
     float cj;

     double ss;
//     char xm[8];
}xc;

struct Login{
    char userName[32];
    char PassWord[32];
};


int main() {
    Login login={"ligang","123456"};
    cout<< sizeof(login)<<endl;
    cout<< sizeof(Login)<<endl;
    return 0;
}
