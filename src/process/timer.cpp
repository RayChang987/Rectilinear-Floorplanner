//
// Created by RayChang on 2023/6/26.
//

#include "timer.h"
#include <iostream>



void timer::timer_start() {
    this->start = std::chrono::high_resolution_clock::now();
}

void timer::timer_end() {
    this->end = std::chrono::high_resolution_clock::now();
}
double timer::get_time_elapsed(){
    std::chrono::duration<double, std::milli> elapsed = this->end - this->start;
    return elapsed.count();
}
void timer::print_time_elapsed(){
    std::chrono::duration<double, std::milli> elapsed = this->end - this->start;
    std::cout<< "timer : "<<this->name<<"->"<<elapsed.count()<<"ms"<<std::endl;
}

timer::timer(std::string name) {
    this->name = name;
}
