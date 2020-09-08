////#define  WIN32_LEAN_AND_MEAN //主要解决WinSock2.h头文件引入问题
//
//#include<iostream>
//#include <vector>
//#include <algorithm>
//#include <map>
//#include <thread>
//#include <mutex>//锁
//#include <atomic> //原子
//#include "ext/CELLTimestamp.h"
//
//using namespace std;
//
//mutex m;
//const int tCount=4;
//atomic_int sum=0;
//
//void workFun(int id){
////    cout<<id<<endl;
//
//    for(int i=0;i<40;i++){
//        m.lock(); //临界区域
////        lock_guard<mutex> lg(m);
////        sum++;
//
//        cout<<id <<" workFun "<<i<<endl;
//        m.unlock();
//    }
//
//}//抢占式
//
//
//int main(){
//
//    thread t[tCount];//线程对象数组
//    for(int i=0;i<tCount;i++){
//        t[i]=thread(workFun,i);
//    }
//
////    CELLTimestamp tTime;
//
//    for(int i=0;i<tCount;i++){
//        t[i].detach();
//    }
//
//
////    t.detach();// 可以和主线程同时执行
////    t.join();  //等待子线程执行完在执行主线程
//
////    cout<<tTime.getElapsedTimeInMilliSec() <<" ms , sum= "<<sum<<endl;
////    sum=0;
////    tTime.update();
////    for(int i=0;i<80000000;i++){
////        sum++;
////    }
////    cout<<tTime.getElapsedTimeInMilliSec() <<" ms , sum= "<<sum<<endl;
//    for(int i=0;i<4;i++){
//        cout<<"main thread"<<endl;
//    }
//
//    while (true){
//
//    }
//
//
//
//    return 0;
//}










#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <fstream>

using namespace std;




vector<string> split3(const string &str, const char pattern)
{
    vector<string> res;
    stringstream ss(str);   //读取str到字符串流中
    string temp;
    //使用getline函数从字符串流中读取,遇到分隔符时停止,和从cin中读取类似
    //注意,getline默认是可以读取空格的
    while (getline(ss, temp, pattern))
    {
        res.push_back(temp);
    }
    return res;
}
int main(){

    int a;
    int index=0;
    fstream fout(".././12.txt",ios::app);
    if(fout.fail()){
        cout<<"open fail!"<<endl;
    }
    cout<<"open success!"<<endl;
    while(cin>>a){
        fout<<"the number are: "<<a<<endl;
        index++;
        if(index==5)
            break;
    }
    fout.close();


    return 0;
}














