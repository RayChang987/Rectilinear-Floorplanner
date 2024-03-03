//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_floorplan_t_H
#define INC_2023PD_floorplan_t_H
#include <vector>
#include <unordered_map>
#include "polygon/polygon_forest_t.h"
#include "utilities/bounding_rectangle_t.h"
#include "static_data/chip_t.h"
#include"static_data/fixed_module_t.h"
#include"static_data/soft_module_t.h"
#include "plugin/visualizer_t.h"


//for debug
#include<iostream>
using std::vector;
using std::unordered_map;
using std::pair;
enum fp_status_t {
    in_progress = 0,
    fail_on_placing_fixed_modules = 1,
    fail_on_placing_soft_modules = 2,
    success_to_place_all_modules = 3
};
class floorplan_t {
    static unordered_map<const module_t*, int> module_to_bd_soft_rect_i_m; // for evaluation
    static unordered_map<const module_t*, int> module_to_bd_fixed_rect_i_m; // for evaluation


    //status
    fp_status_t fp_status = in_progress;
    vector<bool> soft_is_placed;
    vector<uint32_t> soft_deg;




    // evaluation & output
    double score;
    double wirelength;

    void calculate_wirelength();
    double bd_distance(const bounding_rectangle_t& a, const bounding_rectangle_t& b);
    double VE_calculator(const bounding_rectangle_t& bd_rect, pair<const module_t* const, const int> neighbor);

public:
    static  uint32_t min_w_h[2];
    static void init();
    static vector<vector<vec2d_t>> soft_area_to_w_h_m; //area -> (w, h)
    static vector<vec2d_t> find_w_h(uint32_t area);
    floorplan_t();

    vector<bounding_rectangle_t> soft_rects;
    vector<bounding_rectangle_t> fixed_rects;
    polygon_forest_t polygon_forest; // to keep fp valid (the area restriction)
    void cal_soft_deg();
    bool place_soft_module(size_t i, vec2d_t lower_left_pos, vec2d_t size);

    //get set
    const vector<bool>& get_soft_is_placed() const;
    const size_t get_soft_rect_n() const;
    const size_t get_fixed_rect_n() const;
    polygon_forest_t get_polygon_forest();
    double get_wirelength();
    vector<int> get_unplaced_id();
    const vector<uint32_t>& get_soft_deg();
    const double get_score() const;

    //debug
    void print_info(bool);
    void GUI_validation();
};


#endif //INC_2023PD_floorplan_t_H
