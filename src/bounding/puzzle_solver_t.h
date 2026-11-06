//
// Created by 林士傑 on 2023/8/19.
//

#ifndef ICCAD2023PD_PUZZLE_SOLVER_T_H
#define ICCAD2023PD_PUZZLE_SOLVER_T_H

#include "bounding/bounding_line_handler_t.h"

class puzzle_solver_t {
    bounding_line_handler_t handler;
public:
    puzzle_solver_t();
    void solve();
};


#endif //ICCAD2023PD_PUZZLE_SOLVER_T_H
