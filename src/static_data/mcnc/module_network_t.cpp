//
// Created by 林士傑 on 2023/9/18.
//

#include "module_network_t.h"

module_network_t module_network_t::file_input(std::string name, std::fstream &file) {
    module_network_t module_network;
    module_network.instance_name = name;
    file >> module_network.module_name;
    std::string str;
    while(file >> str){
        if(str[str.length() - 1] == ';'){
            module_network.signals.push_back(str.substr(0, str.length() - 1));
            break;
        }
        module_network.signals.push_back(str);
    }
    return module_network;
}
