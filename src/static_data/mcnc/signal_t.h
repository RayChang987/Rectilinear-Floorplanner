//
// Created by 林士傑 on 2023/9/18.
//

#ifndef YALREADER_SIGNAL_T_H
#define YALREADER_SIGNAL_T_H

#include <vector>
#include <string>
#include <fstream>
#include "../../utilities/vec2d_t.h"

class signal_t {
public:
    enum terminal_type_e {
        I,
        O,
        B,
        PI,
        PO,
        PB,
        F,
        PWR,
        GND,
        ERROR
    };
    std::string terminal_type_str[10] = {"I", "O", "B", "PI", "PO", "PB", "F", "PWR", "GND", "ERROR"};
    std::string name;
    terminal_type_e terminal_type = terminal_type_e::ERROR;
    vec2d_t position;
    double width = 0;
    std::string layer;
    double current = 0;
    double voltage = 0;
    static signal_t file_input(std::string name, std::fstream& file);
};


#endif //YALREADER_SIGNAL_T_H
