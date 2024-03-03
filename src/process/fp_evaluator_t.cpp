//
// Created by RayChang on 2023/4/27.
//
#include "fp_evaluator_t.h"
using std::cout;
using std::endl;
uint32_t fp_evaluator_t::get_score(floorplan_t& fp) {
    size_t deg_c = 0;
    int wh = chip_t::get_width()+chip_t::get_height();
//    for(size_t i = 0; i<fp.get_soft_rect_n(); ++i){
//        if(fp.get_soft_is_placed()[i]==false){
//            deg_c+= (fp.get_soft_deg()[i]*wh)+wh;
//        }
//    }

    uint32_t wirelength_cost = fp.get_wirelength()+deg_c;
    return wirelength_cost*0.8;
}



