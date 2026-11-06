#ifndef MCNC_OLD_MODULE_T_H
#define MCNC_OLD_MODULE_T_H

#include <string>
#include <vector>

class mcnc_old_moudle_t {
public:
    std::string name;
    double min_area;
    std::vector<std::string> pins;
};

#endif