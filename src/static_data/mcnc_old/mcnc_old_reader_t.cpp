#include "mcnc_old_reader_t.h"
#include <algorithm>
mcnc_old_moudle_t *mcnc_old_reader_t::get_module_from_name(std::string name) {
    for(auto m : this->modules) {
        if(m->name == name) return m;
    }
    return nullptr;
}

void mcnc_old_reader_t::file_input(std::fstream &blocks_file, std::fstream &nets_file, std::fstream &fixeds_file) {
    this->modules.clear();
    this->connections.clear();

    //blocks file

    int soft_num = 0;
    int hard_num = 0;
    int termi = 0;
    std::string line, word, charac, type_word;
    for (int i = 0 ; i < 5 ; i++){
        std::getline(blocks_file, line);
    }
    while(blocks_file.eof() == 0) {
        blocks_file >> word;
        if (word == "NumSoftRectangularBlocks"){
            blocks_file >> charac;
            blocks_file >> soft_num;
        }else if (word == "NumHardRectilinearBlocks"){
            blocks_file >> charac;
            blocks_file >> hard_num;
        }else if(word == "NumTerminals"){
            blocks_file >> charac;
            blocks_file >> termi;
        }else {
            blocks_file >> type_word;
            if(type_word == "softrectangular"){
                int area = 0;
                blocks_file >> area;
                std::getline(blocks_file, line);
                mcnc_old_moudle_t *m = new mcnc_old_moudle_t();
                m->name = word;
                m->min_area = area;
                m->pins.push_back(word + "pin");
                this->modules.push_back(m);
            }else {
                perror("Error: Invalid type_word");
            }
        }
    }

    //nets file
    int net_num = 0;
    int temp;
    line = "";
    word = "";
    charac = "";
    type_word = "";
    for (int i = 0 ; i < 5 ; i++){
        std::getline(nets_file, line);
    }
    while(nets_file.eof() == 0) {
        nets_file >> word;
        if (word == "NumNets"){
            nets_file >> charac;
            nets_file >> net_num;
        }else if (word == "NumPins"){
            std::getline(nets_file, line);    
        }else if(word == "NetDegree"){
            int deg_num = 0;
            nets_file >> charac;
            nets_file >> deg_num;
            mcnc_old_connection_t* connection = new mcnc_old_connection_t();
            for(int i = 0; i < deg_num; i++){
                nets_file >> word;
                if(word == "#"){
                    std::getline(nets_file, line);
                    i--;
                    continue;
                }
                connection->modules.push_back(this->get_module_from_name(word));
                std::getline(nets_file, line);
            }
            this->connections.push_back(connection);
        }
    }

    //fixed file
    fixeds_file >> this->chipw >> this->chiph;
    while(fixeds_file.eof() == 0) {
        mcnc_old_fixed_module_t* fm = new mcnc_old_fixed_module_t();
        int llx, lly, w, h;
        fixeds_file >> fm->name >> llx >> lly >> w >> h;
        fm->rect = rect_t({llx, lly}, {w, h});
        fm->pins.push_back(fm->name + "pin");
        mcnc_old_moudle_t* m = this->get_module_from_name(fm->name);
        if(m != nullptr) this->modules.erase(std::find(this->modules.begin(), this->modules.end(), m));
    }

}