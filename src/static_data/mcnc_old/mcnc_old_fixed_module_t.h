#ifndef MCNC_OLD_FIXED_MODULE_T_H
#define MCNC_OLD_FIXED_MODULE_T_H

#include <string>
#include <vector>
#include "utilities/rect_t.h"

class mcnc_old_fixed_module_t {
public:
    std::string name;
    rect_t rect;
    std::vector<std::string> pins;
};

#endif