//
// Created by lkj on 2023/4/7.
//

#include "visualizer_t.h"
#include "static_data/chip_t.h"
#include "static_data/module_t.h"
#include <thread>
#include <iostream>
#include <sstream>
#include <iomanip>

int visualizer_t::max_x = 0;
int visualizer_t::max_y = 0;
std::string visualizer_t::name = "Gnuplot window";

void my_out(const std::string& str){
#ifdef USING_VIS
    std::cout << str << std::endl;
#endif
}

void visualizer_t::gnup_th(const std::vector<std::pair<std::vector<vec2d_t>,std::string>> polys_pts, std::string window_name){
    srand(time(NULL));
    int width = visualizer_t::max_x;
    int height = visualizer_t::max_y;
    double canvas_size = 600.0f * double(max_x) / double(max_y);
    FILE* pipe = popen(true ? "gnuplot -persist" : "gnuplot", "w");
    fputs(("set term qt size " + std::to_string(canvas_size) + ",600\n").c_str(), pipe);
    fputs(("set term qt title \"" + window_name + "\"\n").c_str(), pipe);
    fputs("set grid\n", pipe);
    fputs("set mxtics 5\n", pipe);
    fputs("set mytics 5\n", pipe);
    fputs(("set xrange [0:" + std::to_string(width) + "]\n").c_str(), pipe);
    fputs(("set yrange [0:" + std::to_string(height) + "]\n").c_str(), pipe);

    std::string str = "";
    std::stringstream stream;
    for(int m = 0; m < polys_pts.size(); m++){
        str = "set object " + std::to_string(m + 1) + " polygon from ";
        std::vector<vec2d_t> x_y = {};
        for(int i = 0; i < polys_pts[m].first.size(); i++){
            str += std::to_string(polys_pts[m].first[i].get_x());
            str += ",";
            str += std::to_string(polys_pts[m].first[i].get_y());
            str += " to ";
        }
        str += std::to_string(polys_pts[m].first[0].get_x());
        str += ",";
        str += std::to_string(polys_pts[m].first[0].get_y());
        str += " \n";
        fputs(str.c_str(), pipe);

        const uint32_t r = (rand() % 0xBF);
        const uint32_t g = (rand() % 0xBF);
        const uint32_t b = (rand() % 0xBF);
        const uint32_t rgb = r << 16 | g << 8 | b;
        stream.str("");
        stream << "0x"
               << std::setfill ('0') << std::setw(6)
               << std::hex << rgb;

        str = "set object " + std::to_string(m + 1) + " fc rgb " + stream.str() + " "
              "fillstyle transparent solid 0.2 "
              "border lc rgb " + stream.str() + " lw 0.3" " \n";
        fputs(str.c_str(), pipe);
        stream.clear();
        fputs(("set label \"" + polys_pts[m].second + "\" at " +
                std::to_string(polys_pts[m].first[0].get_x()) + "," +
                std::to_string(polys_pts[m].first[0].get_y()) + "\n").c_str(), pipe);
    }

    fputs("plot 0\n", pipe);
    if(pipe != nullptr) { pclose(pipe); }

    //reset window name
}

void visualizer_t::join(const std::vector<std::pair<std::vector<vec2d_t>,std::string>>& polys_pts, std::string window_name){
    std::thread th(visualizer_t::gnup_th, polys_pts, window_name);
    th.detach();
    return;
}

void visualizer_t::draw_bounding_line(const std::vector<std::pair<std::vector<vec2d_t>,std::string>>& bounding_lines) {
    visualizer_t::max_x = chip_t::get_width();
    visualizer_t::max_y = chip_t::get_height();

    join(bounding_lines, visualizer_t::name);
    visualizer_t::set_window_name("Gnuplot window");
    return ;
}

void visualizer_t::show_fp(const std::vector<bounding_rectangle_t>& bd_rect) {
    visualizer_t::max_x = chip_t::get_width();
    visualizer_t::max_y = chip_t::get_height();
    std::vector<std::pair<std::vector<vec2d_t>,std::string>> bd_rect_pt;
    for(auto & i : bd_rect){
        const double rx = i.getRect().get_right_upper().get_x();
        const double lx = i.getRect().get_left_lower().get_x();
        const double uy = i.getRect().get_right_upper().get_y();
        const double ly = i.getRect().get_left_lower().get_y();
        bd_rect_pt.push_back({std::vector<vec2d_t>{{lx, ly}, {rx, ly}, {rx, uy}, {lx,uy}},
                              i.getLinkModule()->getName()});
    }
    join(bd_rect_pt, visualizer_t::name);
    visualizer_t::set_window_name("Gnuplot window");
    return ;
}

void visualizer_t::show_fp_no_border(const std::vector<bounding_rectangle_t>& bd_rect, const std::string& window_name){
    visualizer_t::max_x = chip_t::get_width();
    visualizer_t::max_y = chip_t::get_height();
    std::vector<std::pair<std::vector<vec2d_t>,std::string>> bd_rect_pt;
    for(auto & i : bd_rect){
        const double rx = i.getRect().get_right_upper().get_x();
        const double lx = i.getRect().get_left_lower().get_x();
        const double uy = i.getRect().get_right_upper().get_y();
        const double ly = i.getRect().get_left_lower().get_y();
        if(rx > visualizer_t::max_x) { visualizer_t::max_x = rx; }
        if(uy > visualizer_t::max_y) { visualizer_t::max_y = uy; }
        bd_rect_pt.push_back({std::vector<vec2d_t>{{lx, ly}, {rx, ly}, {rx, uy}, {lx,uy}},
                              i.getLinkModule()->getName()});
    }
    visualizer_t::set_window_name(window_name);
    join(bd_rect_pt, visualizer_t::name);
    visualizer_t::set_window_name("Gnuplot window");
}

void visualizer_t::show_fp_rect_no_border(const std::vector<std::pair<rect_t, std::string>> & rect_pair, const std::string& window_name) {
    visualizer_t::max_x = chip_t::get_width();
    visualizer_t::max_y = chip_t::get_height();
    std::vector<std::pair<std::vector<vec2d_t>,std::string>> bd_rect_pt;
    for(auto & i : rect_pair){
        const double rx = i.first.get_right_upper().get_x();
        const double lx = i.first.get_left_lower().get_x();
        const double uy = i.first.get_right_upper().get_y();
        const double ly = i.first.get_left_lower().get_y();
        if(rx > visualizer_t::max_x) { visualizer_t::max_x = rx; }
        if(uy > visualizer_t::max_y) { visualizer_t::max_y = uy; }
        bd_rect_pt.push_back({std::vector<vec2d_t>{{lx, ly}, {rx, ly}, {rx, uy}, {lx,uy}},
                              i.second});
    }
    visualizer_t::set_window_name(window_name);
    join(bd_rect_pt, visualizer_t::name);
    visualizer_t::set_window_name("Gnuplot window");
}

void visualizer_t::set_window_name(const std::string& name) {
    visualizer_t::name = name;
}