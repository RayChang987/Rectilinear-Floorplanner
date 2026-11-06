//
// Created by 林士傑 on 2023/9/18.
//

#ifndef YALREADER_MODULE_NETWORK_T_H
#define YALREADER_MODULE_NETWORK_T_H

#include <string>
#include <vector>
#include "signal_t.h"

class module_network_t {
public:
    std::string instance_name;
    std::string module_name;
    std::vector<std::string> signals;
    static module_network_t file_input(std::string name, std::fstream& file);
};


#endif //YALREADER_MODULE_NETWORK_T_H
