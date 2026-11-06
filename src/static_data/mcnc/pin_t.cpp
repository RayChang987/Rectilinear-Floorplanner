//
// Created by 林士傑 on 2023/9/20.
//

#include "pin_t.h"

vec2d_t pin_t::get_absolute_position(const vec2d_t &module_position) {
    return module_position + this->relative_position;
}
