//
// Created by 林士傑 on 2023/3/27.
//

#include "bounding_rectangle_t.h"

#include <utility>
#include "static_data/module_t.h"

bounding_rectangle_t::bounding_rectangle_t(rect_t rect) : link_module(nullptr), rect(std::move(rect)) {

}

bounding_rectangle_t::bounding_rectangle_t(module_t *link_module) : link_module(link_module), rect(vec2d_t(0, 0), vec2d_t(0, 0)){

}

bounding_rectangle_t::bounding_rectangle_t(module_t *link_module, rect_t rect) : link_module(link_module), rect(rect){

}


const rect_t &bounding_rectangle_t::getRect() const {
    return rect;
}

const module_t * const bounding_rectangle_t::getLinkModule() const {
    return link_module;
}

bool bounding_rectangle_t::setRect(const rect_t &rect) {
    auto bd = link_module->make_bd(rect);
    if(bd.second){
        this->rect = rect;
    }
    return bd.second;
}
