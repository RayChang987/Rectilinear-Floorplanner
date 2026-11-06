//
// Created by 林士傑 on 2023/9/18.
//

#ifndef YALREADER_MCNC_MODULE_T_H
#define YALREADER_MCNC_MODULE_T_H

#include <string>
#include <vector>
#include <fstream>
#include "signal_t.h"
#include "module_network_t.h"
#include "../../utilities/rect_t.h"
#include "../../utilities/vec2d_t.h"

class mcnc_module_t {
public:
    enum module_type_e {
        STANDARD,
        PAD,
        GENERAL,
        PARENT,
        ERROR
    };
    std::string module_type_str[5] = {"STANDARD", "PAD", "GENERAL", "PARENT", "ERROR"};
    std::string name;
    module_type_e module_type = module_type_e::ERROR;
    std::vector<vec2d_t> dimensions;
    rect_t rect;
    std::vector<signal_t> signals;
    std::vector<module_network_t> network;
    static mcnc_module_t file_input(std::fstream& file);
};


#endif //YALREADER_MCNC_MODULE_T_H
