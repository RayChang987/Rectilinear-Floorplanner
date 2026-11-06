//
// Created by 林士傑 on 2023/9/18.
//

#include <exception>
#include <iostream>
#include "signal_t.h"

signal_t signal_t::file_input(std::string name, std::fstream &file) {
    signal_t signal;
    std::string str;
    signal.name = name;
    file >> str;
    if(str == "I"){
        signal.terminal_type = signal_t::terminal_type_e::I;
    } else if(str == "O"){
        signal.terminal_type = signal_t::terminal_type_e::O;
    } else if(str == "B"){
        signal.terminal_type = signal_t::terminal_type_e::B;
    } else if(str == "PI"){
        signal.terminal_type = signal_t::terminal_type_e::PI;
    } else if(str == "PO"){
        signal.terminal_type = signal_t::terminal_type_e::PO;
    } else if(str == "PB"){
        signal.terminal_type = signal_t::terminal_type_e::PB;
    } else if(str == "F"){
        signal.terminal_type = signal_t::terminal_type_e::F;
    } else if(str == "PWR"){
        signal.terminal_type = signal_t::terminal_type_e::PWR;
    } else if(str == "GND"){
        signal.terminal_type = signal_t::terminal_type_e::GND;
    } else {
        std::cout << str << "   ";
        std::cout << "ERROROROROROROR" << __FILE__ << " " << __LINE__ << "\n";
    }
    if(signal.terminal_type == signal_t::terminal_type_e::PWR){
        double temp_x, temp_y;
        file >> temp_x >> temp_y >> signal.width;
        signal.position = vec2d_t(temp_x, temp_y);
        file >> signal.layer;
        if(signal.layer[signal.layer.length() - 1] != ';'){
            file >> str;
            file >> str;
            if(str[str.length() - 1] == ';'){
                signal.current = std::stod(str.substr(0, str.length() - 1));
            }else{
                signal.current = std::stod(str);
                file >> str;
                file >> str;
                signal.voltage = std::stod(str.substr(0, str.length() - 1));
            }
        }
    }else if(signal.terminal_type == signal_t::terminal_type_e::B || signal.terminal_type == signal_t::terminal_type_e::PB){
        double temp_x, temp_y;
        file >> temp_x >> temp_y >> signal.width;
        signal.position = vec2d_t(temp_x, temp_y);
        file >> str;
        signal.layer = str.substr(0, str.length() - 1);
    }else{
        std::cout << signal.terminal_type << "   ";
        std::cout << "ERROROROROROROR" << __FILE__ << " " << __LINE__ << "\n";
    }
    return signal;
}
