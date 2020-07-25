//#define  WIN32_LEAN_AND_MEAN //主要解决WinSock2.h头文件引入问题

#include<iostream>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;
vector<int> res;
int num=0;
int tmp;

void test(vector<int> &arr){
    sort(arr.begin(),arr.end());
    int j=arr.size()-2;
    for(int i=arr.size()-1;i>=0;i--,j--){
        if(arr[i]==arr[j] ){
            if(arr[i]==tmp){
                continue;
            }
            res.push_back(arr[i]);
            tmp=arr[i];
            num++;
            if(num==2)
                return;
        }
    }
}

map<int,int> mm;
void test2(vector<int> &arr){
    for(auto it:arr){
        mm[it]++;
    }
}

int main(){

    int num;
    cin>>num;
    int tmp;
    vector<int> data;
    while (cin>>tmp)
    {
        data.push_back(tmp);
        cout<<tmp<<endl;
    }

    for(int i=0;i<data.size();i++){
        cout<<data[i]<<" ";
    }

    cout<<endl;
    return 0;
}
