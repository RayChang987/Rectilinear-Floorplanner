//
// Created by lkj on 2023/3/27.
//

#ifndef INC_2023PD_VISUALIZER_T_H
#define INC_2023PD_VISUALIZER_T_H

#include <vector>
#include <string>
#include "utilities/bounding_rectangle_t.h"

class visualizer_t {
public:
    static void show_fp(const std::vector<bounding_rectangle_t>&);
    static void draw_bounding_line(const std::vector<std::pair<std::vector<vec2d_t>,std::string>>& bounding_lines, std::string svg_path = "");
    static void show_fp_no_border(const std::vector<bounding_rectangle_t>&, const std::string&);
    static void show_fp_rect_no_border(const std::vector<std::pair<rect_t, std::string>> &, const std::string&);
    static void set_window_name(const std::string&);
    static void draw_bounding_line_connection(std::vector<std::pair<std::vector<vec2d_t>,std::string>> bounding_lines);
    static void polygon_detail(const std::vector<vec2d_t>& points, std::string window_name);
private:
    static int max_x;
    static int max_y;
    static std::string name;
    static void join(const std::vector<std::pair<std::vector<vec2d_t>,std::string>>& polys_pts, std::string window_name, std::string svg_path = "");
    static void gnup_th(const std::vector<std::pair<std::vector<vec2d_t>,std::string>> polys_pts, std::string window_name, std::string svg_path = "");
    static void polygon_detail_th(const std::vector<vec2d_t>& points, std::string window_name);
};


#endif //INC_2023PD_VISUALIZER_T_H
