//
// Created by 林士傑 on 2023/3/27.
//

#include <random>
#include "quadratic_t.h"
#include "static_data/chip_t.h"
std::vector<vec2d_t> quadratic_t::final_position;
std::vector<vec2d_t> quadratic_t::slope;
std::vector<vec2d_t> quadratic_t::constrain;

std::default_random_engine quadratic_t::rd;
std::mt19937 quadratic_t::gen(quadratic_t::rd());

vec2d_t quadratic_t::get_coor(int index) {
    return final_position[index];
}

vec2d_t quadratic_t::get_coor_random(int index) {
    return {rand()%chip_t::get_width(), rand()%chip_t::get_height()};
}

void quadratic_t::set_floopplanning(const std::vector<bounding_rectangle_t> &bounding_rectangles,
                                    const std::vector<bool> &is_placed) {
}
