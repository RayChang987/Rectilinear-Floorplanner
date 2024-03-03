//
// Created by 林士傑 on 2023/3/27.
//

#include "soft_module_t.h"

soft_module_t soft_module_t::void_module = soft_module_t(0);

soft_module_t::soft_module_t() : minArea(0) {
    this->name = "";
}

soft_module_t::soft_module_t(const int& area) : minArea(area) {
    this->name = "";
}

soft_module_t *soft_module_t::fileInput(std::fstream &file) {
    soft_module_t* soft_module = new soft_module_t();
    file >> soft_module->name >> soft_module->minArea;
    return soft_module;
}

const uint32_t soft_module_t::get_area() const {
    return static_cast<const uint32_t>(this->minArea);
}

const vec2d_t soft_module_t::get_left_lower() const {
    return vec2d_t::default_position;
}

const vec2d_t soft_module_t::get_size() const {
    return vec2d_t(std::sqrt(this->minArea) / 2);
}

bounding_rectangle_t soft_module_t::make_bd() {
    return {this};
}

std::pair<bounding_rectangle_t, bool> soft_module_t::make_bd(const rect_t &rect) const {
    if(rect.get_size().get_area() < this->minArea)
        return {{(module_t*)this, rect}, false};
    if(std::min(rect.get_size().get_x(), rect.get_size().get_y()) * 2 < std::max(rect.get_size().get_x(), rect.get_size().get_y()))
        return {{(module_t*)this, rect}, false};
    return {{(module_t*)this, rect}, true};
}
