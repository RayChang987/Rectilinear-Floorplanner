//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_SOFT_MODULE_T_H
#define INC_2023PD_SOFT_MODULE_T_H

#include "static_data/module_t.h"

class soft_module_t : public module_t {
    friend class chip_t;
    int minArea;
    soft_module_t();
    soft_module_t(const int&);
public:
    rect_t rect;
    static soft_module_t void_module;

    const uint32_t get_area() const override;

    const vec2d_t get_left_lower() const override;

    const vec2d_t get_size() const override;

    bounding_rectangle_t make_bd() override;

    std::pair<bounding_rectangle_t, bool> make_bd(const rect_t &rect) const override;

    static soft_module_t* fileInput(std::fstream& file);
};


#endif //INC_2023PD_SOFT_MODULE_T_H
