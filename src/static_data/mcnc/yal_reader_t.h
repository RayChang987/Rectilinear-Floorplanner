//
// Created by 林士傑 on 2023/9/18.
//

#ifndef YALREADER_YAL_READER_T_H
#define YALREADER_YAL_READER_T_H

#include <string>
#include <fstream>
#include "mcnc_module_t.h"

class yal_reader_t {
public:
    std::vector<mcnc_module_t> modules;
    void file_input(std::fstream& file);
    void file_saver(std::string fileName);
};


#endif //YALREADER_YAL_READER_T_H
