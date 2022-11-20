#pragma once

#include <chrono>
#include <iostream>
#define MILLISCALE 1000

using namespace std::chrono;
typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> fsec;
typedef Time::time_point time_stamp;

static time_stamp get_curr_time(){
    return  Time::now();
}

static float get_time_diff(const time_stamp& t1, const time_stamp& t2){
    fsec fs = t2 - t1;
    ms d = duration_cast<ms>(fs);
    return float(fs.count());
}

