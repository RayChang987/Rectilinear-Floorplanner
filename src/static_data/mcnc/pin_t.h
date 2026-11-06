//
// Created by 林士傑 on 2023/9/20.
//

#ifndef YALREADER_PIN_T_H
#define YALREADER_PIN_T_H

#include <string>
#include "utilities/vec2d_t.h"

class module_t;
class multi_net_t;

class pin_t {
public:
    std::string name;
    vec2d_t relative_position;
    int module_index;
    module_t* belong_module;
    multi_net_t* connect_net;

    vec2d_t get_absolute_position(const vec2d_t& module_position);
};


#endif //YALREADER_PIN_T_H
