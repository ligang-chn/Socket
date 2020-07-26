//
// Created by ligang on 2020/7/26.
//

#ifndef SOCKET_CELLTIMESTAMP_H
#define SOCKET_CELLTIMESTAMP_H

#include <chrono>

using namespace std::chrono;

class CELLTimestamp {
public:
    CELLTimestamp();

    ~CELLTimestamp();

    void update();
    //获取秒
    double getElapsedSecond();

    //获取毫秒
    double getElapsedTimeInMilliSec();

    //获取微秒
    long long getElapsedTimeInMicroSec();

protected:
    time_point<high_resolution_clock> _begin;
};


#endif //SOCKET_CELLTIMESTAMP_H
