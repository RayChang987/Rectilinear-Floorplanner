//
// Created by 林士傑 on 2023/9/18.
//

#include <exception>
#include <iostream>
#include "yal_reader_t.h"
#include <iomanip>

void yal_reader_t::file_input(std::fstream &file) {
    std::string str;
    while(file >> str){
        if(str == "MODULE"){
            this->modules.push_back(mcnc_module_t::file_input(file));
        }else{
            std::cout << str << "\n";
            std::cout << "ERROROROROROROR" << __FILE__ << " " << __LINE__ << "\n";
        }
    }
}

void yal_reader_t::file_saver(std::string fileName) {
    std::fstream file;
    file.open(fileName, std::ios::out);

    if(file.fail()){
        file.close();
        std::cout << "failed to open \"" << fileName << "\"" << std::endl;
        return;
    }

    file << std::fixed << std::setprecision(3);

    for(auto module : this->modules) {
        file << "MODULE " << module.name << ";\n";
        file << " TYPE " << module.module_type_str[module.module_type] << ";\n";
        auto bd = module.rect.to_bounding_point();
        file << " DIMENSIONS "
            << bd[3].get_x() << " " << bd[3].get_y() << " "
            << bd[2].get_x() << " " << bd[2].get_y() << " "
            << bd[1].get_x() << " " << bd[1].get_y() << " "
            << bd[0].get_x() << " " << bd[0].get_y() << ";\n";
        file << " IOLIST;\n";
        for(auto si : module.signals) {
            file << "  "
                << si.name << " "
                << si.terminal_type_str[si.terminal_type] << " "
                << si.position.get_x() << " "
                << si.position.get_y() << " "
                << si.width << " "
                << si.layer;
            if(si.voltage != 0) 
                file << " VOLTAGE " << si.voltage;
            if(si.current != 0)
                file << " CURRENT " << si.current;
            file << ";\n";
        }
        file << " ENDIOLIST;\n";

        if(module.network.size() != 0) {
            file << " NETWORK;\n";

            for(auto n : module.network) {
                file << "  "
                    << n.instance_name << " "
                    << n.module_name;
                
                for(auto s : n.signals) {
                    file << " " << s;
                }
                file << ";\n";
            }

            file << " ENDNETWORK;\n";
        }

        file << "ENDMODULE;\n";
    }
    
    file.close();
}
