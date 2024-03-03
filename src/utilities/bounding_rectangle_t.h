//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_BOUNDING_RECTANGLE_T_H
#define INC_2023PD_BOUNDING_RECTANGLE_T_H

#include "utilities/rect_t.h"

class module_t;

class bounding_rectangle_t {
    friend class module_t;
    friend class fixed_module_t;
    friend class soft_module_t;
    //evil
    friend class polygon_forest_t;
    friend class polygon_t;
    //evil
    module_t* link_module;
    rect_t rect;

    bounding_rectangle_t(rect_t rect);

    bounding_rectangle_t(module_t* link_module);

    bounding_rectangle_t(module_t* link_module, rect_t rect);
public:
    bounding_rectangle_t() = delete;

    const rect_t &getRect() const;

    bool setRect(const rect_t& rect);

    const module_t * const getLinkModule() const;
};


#endif //INC_2023PD_BOUNDING_RECTANGLE_T_H
