//
// Created by 林士傑 on 2023/3/27.
//

#include "fixed_module_t.h"
#include "utilities/rect_t.h"
#include "utilities/bounding_rectangle_t.h"

fixed_module_t::fixed_module_t() {

}

fixed_module_t *fixed_module_t::fileInput(std::fstream &file) {
    fixed_module_t* fixedModule = new fixed_module_t();
    file >> fixedModule->name >> fixedModule->xCoord >> fixedModule->yCoord >> fixedModule->width >> fixedModule->height;
    fixedModule->rect = new rect_t(vec2d_t(fixedModule->xCoord, fixedModule->yCoord),
                                   vec2d_t(fixedModule->width, fixedModule->height));
    return fixedModule;
}

const uint32_t fixed_module_t::get_area() const {
    return this->rect == nullptr? 0 : static_cast<const uint32_t>(this->rect->get_area());
}

const vec2d_t fixed_module_t::get_left_lower() const {
    return this->rect->get_left_lower();
}

const vec2d_t fixed_module_t::get_size() const {
    return this->rect->get_size();
}

bounding_rectangle_t fixed_module_t::make_bd() {
    return {this, *this->rect};
}

std::pair<bounding_rectangle_t, bool> fixed_module_t::make_bd(const rect_t& rect) const {
    return {{(module_t*)this, *this->rect}, *this->rect == rect};
}