//
// Created by lkj on 2023/4/7.
//

#include "visualizer_t.h"
#include "static_data/chip_t.h"
#include "static_data/module_t.h"
#include <csignal>
#include <cstdlib>
#include <thread>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace {

// Plots are drawn by gnuplot. Without it, writing to the pipe would raise
// SIGPIPE and kill the program, so plotting is skipped (with one warning).
bool gnuplot_available() {
    static const bool available = [] {
        std::signal(SIGPIPE, SIG_IGN);
        const bool found =
            std::system("command -v gnuplot > /dev/null 2>&1") == 0;
        if (!found) {
            std::cerr << "[Warning] gnuplot not found: plots are skipped"
                      << std::endl;
        }
        return found;
    }();
    return available;
}

}  // namespace

int visualizer_t::max_x = 0;
int visualizer_t::max_y = 0;
std::string visualizer_t::name = "Gnuplot window";

void my_out(const std::string& str){
#ifdef USING_VIS
    std::cout << str << std::endl;
#endif
}

void visualizer_t::gnup_th(const std::vector<std::pair<std::vector<vec2d_t>,std::string>> polys_pts, std::string window_name, std::string svg_path){
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

    if(svg_path != "") {
        fputs("set term svg\n", pipe);
        fputs(("set output \"" + svg_path + "\"\n").c_str(), pipe);
    }

    fputs("plot 0\n", pipe);
    if(pipe != nullptr) { pclose(pipe); }

    //reset window name
}

void visualizer_t::polygon_detail_th(const std::vector<vec2d_t>& points, std::string window_name) {
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
    str = "set object 1 polygon from ";
    for(int m = 0; m < points.size(); m++){
        str += std::to_string(points[m].get_x());
        str += ",";
        str += std::to_string(points[m].get_y());
        str += " to ";
    }
    str += std::to_string(points[0].get_x());
    str += ",";
    str += std::to_string(points[0].get_y());
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

    str = "set object 1 fc rgb " + stream.str() + " "
            "fillstyle transparent solid 0.2 "
            "border lc rgb " + stream.str() + " lw 0.3" " \n";
    fputs(str.c_str(), pipe);
    stream.clear();
    for(int i = 0; i < points.size(); ++i) {
        fputs(("set label \"" + std::to_string(i) + "\" at " +
            std::to_string(points[i].get_x()) + "," +
            std::to_string(points[i].get_y()) + "\n").c_str(), pipe);
    }
        
    fputs("plot 0\n", pipe);
    if(pipe != nullptr) { pclose(pipe); }
}

void visualizer_t::join(const std::vector<std::pair<std::vector<vec2d_t>,std::string>>& polys_pts, std::string window_name, std::string svg_path){
    if (!gnuplot_available()) { return; }
    std::thread th(visualizer_t::gnup_th, polys_pts, window_name, svg_path);
    th.detach();
    return;
}

void visualizer_t::draw_bounding_line(const std::vector<std::pair<std::vector<vec2d_t>,std::string>>& bounding_lines, std::string svg_path) {
    visualizer_t::max_x = chip_t::get_width();
    visualizer_t::max_y = chip_t::get_height();

    // visualizer_t::max_x = 11267;
    // visualizer_t::max_y =  10450;
    join(bounding_lines, visualizer_t::name, svg_path);
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

vec2d_t get_center(std::vector<vec2d_t> points) {
    vec2d_t ll(1e9, 1e9), ru(-1e9, -1e9);
    for(auto point : points){
        ll.set_x(std::min(ll.get_x(), point.get_x()));
        ll.set_y(std::min(ll.get_y(), point.get_y()));
        ru.set_x(std::max(ru.get_x(), point.get_x()));
        ru.set_y(std::max(ru.get_y(), point.get_y()));
    }
    return (ll + ru) / 2;
}

void visualizer_t::draw_bounding_line_connection(std::vector<std::pair<std::vector<vec2d_t>,std::string>> bounding_lines) {
    auto table = chip_t::get_connection_table();
    auto max_value = -1.0e9;
    auto min_value = 1.0e9;
    auto width_min = 1.0;
    auto width_max = 0.02 * std::min(chip_t::get_width(), chip_t::get_height());
    for(auto tab : table){
        for(auto ta : tab){
            min_value = std::min(min_value, (double)ta);
            max_value = std::max(max_value, (double)ta);
        }
    }
    for(int i = 0; i < table.size(); ++i) {
        for(int j = i + 1; j < table[i].size(); ++j) {
            auto st = (bounding_lines[i].first[0] + bounding_lines[i].first[2]) / 2.0;
            auto en = (bounding_lines[j].first[0] + bounding_lines[j].first[2]) / 2.0;
            auto slope_vec = en - st;
            auto rvs_vec = vec2d_t(-slope_vec.get_y(), slope_vec.get_x());
            rvs_vec = rvs_vec / rvs_vec.get_length();
            rvs_vec = rvs_vec * ((table[i][j] - min_value) / (max_value - min_value) * (width_max - width_min) + width_min);
            if(table[i][j] != 0)
                bounding_lines.push_back({{st - rvs_vec, st + rvs_vec, en + rvs_vec, en - rvs_vec}, std::to_string(table[i][j])});
        }
    }
    visualizer_t::draw_bounding_line(bounding_lines);
    return ;
}

void visualizer_t::polygon_detail(const std::vector<vec2d_t>& points, std::string window_name) {
    visualizer_t::max_x = chip_t::get_width();
    visualizer_t::max_y = chip_t::get_height();
    if (!gnuplot_available()) { return; }
    std::thread th(visualizer_t::polygon_detail_th, points, window_name);
    th.detach();
    return;
}
