//
// Created by 林士傑 on 2023/9/18.
//

#include "mcnc_module_t.h"

mcnc_module_t mcnc_module_t::file_input(std::fstream& file) {
    mcnc_module_t m;
    std::string str;
    file >> str;
    m.name = str.substr(0, str.length() - 1);
    while(file >> str){
        if (str == "TYPE"){
            file >> str;
            if (str == "GENERAL;"){
                m.module_type = mcnc_module_t::module_type_e::GENERAL;
            } else if(str == "PAD;") {
                m.module_type = mcnc_module_t::module_type_e::PAD;
            } else if(str == "GENERAL;") {
                m.module_type = mcnc_module_t::module_type_e::GENERAL;
            } else if(str == "PARENT;") {
                m.module_type = mcnc_module_t::module_type_e::PARENT;
            } else {
                m.module_type = mcnc_module_t::module_type_e::ERROR;
            }
        } else if (str == "DIMENSIONS"){
            double temp_x, temp_y;
            file >> str; temp_x = std::stod(str);
            file >> str; temp_y = std::stod(str);
            m.dimensions.emplace_back(vec2d_t(temp_x, temp_y));
            file >> str; temp_x = std::stod(str);
            file >> str; temp_y = std::stod(str);
            m.dimensions.emplace_back(temp_x, temp_y);
            file >> str; temp_x = std::stod(str);
            file >> str; temp_y = std::stod(str);
            m.dimensions.emplace_back(temp_x, temp_y);
            file >> str; temp_x = std::stod(str);
            file >> str; temp_y = std::stod(str.substr(0, str.length() - 1));
            m.dimensions.emplace_back(temp_x, temp_y);
            double min_x = 1e10, min_y = 1e10, max_x = -1e10, max_y = -1e10;
            for (auto di : m.dimensions){
                min_x = std::min(min_x, di.get_x());
                min_y = std::min(min_y, di.get_y());
                max_x = std::max(max_x, di.get_x());
                max_y = std::max(max_y, di.get_y());
            }
            m.rect = rect_t(vec2d_t(min_x, min_y), vec2d_t(max_x - min_x, max_y - min_y));
        } else if (str == "IOLIST;"){
            file >> str;
            while(str != "ENDIOLIST;"){
                m.signals.push_back(signal_t::file_input(str, file));
                file >> str;
            }
        } else if (str == "NETWORK;"){
            file >> str;
            while(str != "ENDNETWORK;"){
                m.network.push_back(module_network_t::file_input(str, file));
                file >> str;
            }
        } else if (str == "ENDMODULE;"){
            break;
        }
    }
    return m;
}
