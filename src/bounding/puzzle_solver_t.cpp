//
// Created by 林士傑 on 2023/8/19.
//

#include "puzzle_solver_t.h"
#include "static_data/chip_t.h"
#include "static_data/fixed_module_t.h"
#include "static_data/soft_module_t.h"

puzzle_solver_t::puzzle_solver_t() : handler(vec2d_t(chip_t::get_width(), chip_t::get_height())) {

}

void puzzle_solver_t::solve() {
    auto fix_modules = chip_t::get_fixed_modules();
    for(auto fix : fix_modules){
        auto f = fix->make_bd();
        this->handler.add_bounding_line(bounding_line_t(f.getRect()));
    }
    
}
