//
// Created by RayChang on 2023/6/26.
//

#ifndef ICCAD2023PD_TIMER_H
#define ICCAD2023PD_TIMER_H
#include <ctime>
#include <chrono>
#include <string>
class timer {
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    std::string name;
public:
    timer(std::string);

    void timer_start();

    void timer_end();

    double get_time_elapsed();

    void print_time_elapsed();
};


#endif //ICCAD2023PD_TIMER_H
