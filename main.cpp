//#define  WIN32_LEAN_AND_MEAN //��Ҫ���WinSock2.hͷ�ļ���������

#include<iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <thread>
#include <mutex>//��
#include <atomic> //ԭ��
#include "ext/CELLTimestamp.h"

using namespace std;

mutex m;
const int tCount=4;
atomic_int sum=0;

void workFun(int id){
//    cout<<id<<endl;

    for(int i=0;i<20000000;i++){
//        m.lock(); //�ٽ�����
//        lock_guard<mutex> lg(m);
        sum++;

//        cout<<i <<" workFun "<<id<<endl;
//        m.unlock();
    }

}//��ռʽ


int main(){

    thread t[tCount];
    for(int i=0;i<tCount;i++){
        t[i]=thread(workFun,i);
    }

    CELLTimestamp tTime;

    for(int i=0;i<tCount;i++){
        t[i].join();
    }
//    t.detach();// ���Ժ����߳�ͬʱִ��
//    t.join();  //�ȴ����߳�ִ������ִ�����߳�

    cout<<tTime.getElapsedTimeInMilliSec() <<" ms , sum= "<<sum<<endl;
    sum=0;
    tTime.update();
    for(int i=0;i<80000000;i++){
        sum++;
    }
    cout<<tTime.getElapsedTimeInMilliSec() <<" ms , sum= "<<sum<<endl;

    cout<<"hello thread"<<endl;




    return 0;
}
