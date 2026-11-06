//
// Created by 林士傑 on 2023/9/20.
//

#ifndef YALREADER_MULTI_NET_T_H
#define YALREADER_MULTI_NET_T_H

#include <vector>
#include "utilities/vec2d_t.h"
#include <string>
class pin_t;

class multi_net_t {
public:
    std::vector<pin_t*> pins;
    int weight = 1;
    std::string name;
    double calculate_wire_length(const std::vector<vec2d_t> module_left_lower_position);
};


#endif //YALREADER_MULTI_NET_T_H
