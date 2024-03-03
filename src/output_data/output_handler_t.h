//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_OUTPUT_HANDLER_T_H
#define INC_2023PD_OUTPUT_HANDLER_T_H

#include <vector>
#include <string>
#include "polygon/polygon_forest_t.h"
#include "polygon/polygon_t.h"
#include "process/floorplan_t.h"
#include "output_data/output_utility_t.h"

class output_handler_t {
    static double hpwl;
    static std::vector<output_utility_t> utilities;
    //todo: wait upgrade
    static void parse_polygon(polygon_t& polygon);
    static void test(polygon_forest_t& polygonForest);
public:
    static void ttt();
    static void set_FP(floorplan_t& floorplanning);
    static double HPWL();
    static std::vector<output_utility_t> to_real();
};


#endif //INC_2023PD_OUTPUT_HANDLER_T_H
