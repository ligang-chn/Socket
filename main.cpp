////#define  WIN32_LEAN_AND_MEAN //��Ҫ���WinSock2.hͷ�ļ���������
//
//#include<iostream>
//#include <vector>
//#include <algorithm>
//#include <map>
//#include <thread>
//#include <mutex>//��
//#include <atomic> //ԭ��
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
//        m.lock(); //�ٽ�����
////        lock_guard<mutex> lg(m);
////        sum++;
//
//        cout<<id <<" workFun "<<i<<endl;
//        m.unlock();
//    }
//
//}//��ռʽ
//
//
//int main(){
//
//    thread t[tCount];//�̶߳�������
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
////    t.detach();// ���Ժ����߳�ͬʱִ��
////    t.join();  //�ȴ����߳�ִ������ִ�����߳�
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
    stringstream ss(str);   //��ȡstr���ַ�������
    string temp;
    //ʹ��getline�������ַ������ж�ȡ,�����ָ���ʱֹͣ,�ʹ�cin�ж�ȡ����
    //ע��,getlineĬ���ǿ��Զ�ȡ�ո��
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














