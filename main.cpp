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


union test
{
    int i;
    char x[2];
};


int main(){

    test a;
    a.x[0]=10;
    a.x[1]=1;
    bool aa=(a.x[0]==a.i);
    cout<<aa;
    return 0;
}