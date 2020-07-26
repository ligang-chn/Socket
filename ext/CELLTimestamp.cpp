//
// Created by ligang on 2020/7/26.
//

#include "CELLTimestamp.h"

CELLTimestamp::CELLTimestamp(){
    update();
}

CELLTimestamp::~CELLTimestamp() {

}

void CELLTimestamp::update(){
    _begin=high_resolution_clock::now();
}

double CELLTimestamp::getElapsedSecond(){
    return getElapsedTimeInMicroSec()*0.000001;
}

double CELLTimestamp::getElapsedTimeInMilliSec(){
    return getElapsedTimeInMicroSec()*0.001;
}

//获取微秒
long long CELLTimestamp::getElapsedTimeInMicroSec(){
    return duration_cast<microseconds>(high_resolution_clock::now()-_begin).count();
}