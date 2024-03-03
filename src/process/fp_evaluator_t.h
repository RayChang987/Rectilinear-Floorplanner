//
// Created by RayChang on 2023/4/27.
//

#ifndef ICCAD2023PD_FP_EVALUATOR_T_H
#define ICCAD2023PD_FP_EVALUATOR_T_H

#include "floorplan_t.h"
class fp_evaluator_t{
public:
    static uint32_t get_score(floorplan_t&);
    static uint32_t get_dead_space(floorplan_t&);
};

#endif //ICCAD2023PD_FP_EVALUATOR_T_H
