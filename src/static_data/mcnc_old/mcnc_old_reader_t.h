#ifndef MCNC_OLD_READER_T_H
#define MCNC_OLD_READER_T_H

#include <string>
#include <fstream>
#include <vector>
#include "mcnc_old_moudle_t.h"
#include "mcnc_old_fixed_module_t.h"
#include "mcnc_old_connection_t.h"

class mcnc_old_reader_t {
    mcnc_old_moudle_t* get_module_from_name(std::string name);
public:
    double chipw, chiph;
    std::vector<mcnc_old_moudle_t*> modules;
    std::vector<mcnc_old_fixed_module_t*> fix_modules;
    std::vector<mcnc_old_connection_t*> connections;
    void file_input(std::fstream& blocks_file, std::fstream& nets_file, std::fstream &fixeds_file);
};

#endif