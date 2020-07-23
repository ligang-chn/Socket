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

    int n,a;
    cin>>n;
    vector<int> answ;
    n--;
    while(cin>>a && n){
        answ.push_back(a);
        n--;
    }

//    test(answ);
    test2(answ);
    int l=0,w=0;
    int tmp=0;
    int t=0;
    vector<int> answw;
    for(auto it:mm){
        if(it.second>1){
            if(it.second>3){
                return it.first*it.first;
            }
            answw.push_back(it.first);
        }

    }




    int sqrm=0;
    sqrm=answw[0]*answw[1];
    cout<<sqrm<<endl;
    return 0;
}
