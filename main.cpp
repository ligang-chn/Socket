//#define  WIN32_LEAN_AND_MEAN //主要解决WinSock2.h头文件引入问题

#include<iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <thread>
#include <mutex>//锁
#include <atomic> //原子
#include "ext/CELLTimestamp.h"

using namespace std;

mutex m;
const int tCount=4;
atomic_int sum=0;

void workFun(int id){
//    cout<<id<<endl;

    for(int i=0;i<20000000;i++){
//        m.lock(); //临界区域
//        lock_guard<mutex> lg(m);
        sum++;

//        cout<<i <<" workFun "<<id<<endl;
//        m.unlock();
    }

}//抢占式


int main(){

    thread t[tCount];
    for(int i=0;i<tCount;i++){
        t[i]=thread(workFun,i);
    }

    CELLTimestamp tTime;

    for(int i=0;i<tCount;i++){
        t[i].join();
    }
//    t.detach();// 可以和主线程同时执行
//    t.join();  //等待子线程执行完在执行主线程

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
