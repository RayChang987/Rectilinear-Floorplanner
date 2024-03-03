//
// Created by 林士傑 on 2023/3/27.
//

#include "output_handler_t.h"
#include "carver_t.h"

double output_handler_t::hpwl;

std::vector<output_utility_t> output_handler_t::utilities;

void output_handler_t::parse_polygon(polygon_t &polygon) {
    std::map<polygon_module_t*, output_utility_t> maps;
    for (auto rect : polygon.get_rects()){
        if(rect->get_area_from_where().size() == 1){
            for(auto area : rect->get_area_from_where()){
                maps[area.first] = output_utility_t(area.first->get_module_bounding_rectangle().getLinkModule()->getName(), {
                        rect->get_module_bounding_rectangle().getRect().get_left_lower(),
                        vec2d_t(rect->get_module_bounding_rectangle().getRect().get_right_upper().get_x(),
                                rect->get_module_bounding_rectangle().getRect().get_left_lower().get_y()),
                        rect->get_module_bounding_rectangle().getRect().get_right_upper(),
                        vec2d_t(rect->get_module_bounding_rectangle().getRect().get_left_lower().get_x(),
                                rect->get_module_bounding_rectangle().getRect().get_right_upper().get_y())
                });
                for(auto conn : area.first->connections){
                    maps[area.first].minus_new_points(line_t::to_lines({
                       conn->get_module_bounding_rectangle().getRect().get_left_lower(),
                       vec2d_t(conn->get_module_bounding_rectangle().getRect().get_right_upper().get_x(),
                               conn->get_module_bounding_rectangle().getRect().get_left_lower().get_y()),
                       conn->get_module_bounding_rectangle().getRect().get_right_upper(),
                       vec2d_t(conn->get_module_bounding_rectangle().getRect().get_left_lower().get_x(),
                               conn->get_module_bounding_rectangle().getRect().get_right_upper().get_y())
                    }));
                    int tt = 0;
                }
                int z = 0;
            }
        }
        int j = 0;
    }
    for (auto overlap : polygon.get_overlap_unit()){
        std::pair<polygon_module_t*, int32_t>* left_top = nullptr;
        std::pair<polygon_module_t*, int32_t>* right_bottom = nullptr;

        std::pair<polygon_module_t*, int32_t>* left_bottom = nullptr;
        std::pair<polygon_module_t*, int32_t>* right_top = nullptr;

        std::vector<std::pair<polygon_module_t*, int32_t>> areas(overlap->area_from_where.begin(), overlap->area_from_where.end());
        auto fir = areas[0].first->get_bounding_rect().get_center();
        auto sec = areas[1].first->get_bounding_rect().get_center();
        if(fir.get_x() <= sec.get_x() && fir.get_y() >= sec.get_y()){
            left_top = &areas[0];
            right_bottom = &areas[1];
        }else if(sec.get_x() <= fir.get_x() && sec.get_y() >= fir.get_y()){
            left_top = &areas[1];
            right_bottom = &areas[0];
        }else if(fir.get_x() <= sec.get_x() && fir.get_y() <= sec.get_y()){
            left_bottom = &areas[0];
            right_top = &areas[1];
        }else if(sec.get_x() <= fir.get_x() && sec.get_y() <= fir.get_y()){
            left_bottom = &areas[1];
            right_top = &areas[0];
        }

        if(left_top != nullptr){
            int fir_row_count = 0, fir_left_count = 0, sec_row_count = 0, sec_left_count = 0;
            fir_row_count = left_top->second / overlap->get_bounding_rect().get_size().get_x();
            fir_left_count = left_top->second % (int)std::round(overlap->get_bounding_rect().get_size().get_x());
            sec_row_count = right_bottom->second / overlap->get_bounding_rect().get_size().get_x();
            sec_left_count = right_bottom->second % (int)std::round(overlap->get_bounding_rect().get_size().get_x());
            std::vector<vec2d_t> point_fir;
            std::vector<vec2d_t> point_sec;
            if(fir_row_count > 0 || fir_left_count > 0){
                point_fir.push_back(vec2d_t(overlap->get_module_bounding_rectangle().getRect().get_left_lower().get_x(),
                                            overlap->get_module_bounding_rectangle().getRect().get_right_upper().get_y()));
                point_fir.push_back(point_fir.back() + vec2d_t(0, - fir_row_count - (fir_left_count > 0 ? 1 : 0)));
                if(fir_left_count > 0){
                    point_fir.push_back(point_fir.back() + vec2d_t(fir_left_count, 0));
                    point_fir.push_back(point_fir.back() + vec2d_t(0, 1));
                }
                if(fir_row_count > 0){
                    point_fir.push_back(vec2d_t(overlap->get_bounding_rect().get_right_upper().get_x(), point_fir.back().get_y()));
                    point_fir.push_back(overlap->get_bounding_rect().get_right_upper());
                }
                maps[left_top->first].add_new_points(line_t::to_lines(point_fir));
            }
            if(sec_row_count > 0 || sec_left_count > 0){
                point_sec.push_back(vec2d_t(overlap->get_module_bounding_rectangle().getRect().get_right_upper().get_x(),
                                            overlap->get_module_bounding_rectangle().getRect().get_left_lower().get_y()));
                point_sec.push_back(point_sec.back() + vec2d_t(0, sec_row_count + (sec_left_count > 0 ? 1 : 0)));
                if(sec_left_count > 0){
                    point_sec.push_back(point_sec.back() + vec2d_t(-sec_left_count, 0));
                    point_sec.push_back(point_sec.back() + vec2d_t(0, -1));
                }
                if(sec_row_count > 0){
                    point_sec.push_back(vec2d_t(overlap->get_bounding_rect().get_left_lower().get_x(), point_sec.back().get_y()));
                    point_sec.push_back(overlap->get_bounding_rect().get_left_lower());
                }
                maps[right_bottom->first].add_new_points(line_t::to_lines(point_sec));
            }
        }else{
            int fir_row_count = 0, fir_left_count = 0, sec_row_count = 0, sec_left_count = 0;
            fir_row_count = left_bottom->second / overlap->get_bounding_rect().get_size().get_x();
            fir_left_count = left_bottom->second % (int)std::round(overlap->get_bounding_rect().get_size().get_x());
            sec_row_count = right_top->second / overlap->get_bounding_rect().get_size().get_x();
            sec_left_count = right_top->second % (int)std::round(overlap->get_bounding_rect().get_size().get_x());
            std::vector<vec2d_t> point_fir;
            std::vector<vec2d_t> point_sec;
            if(fir_row_count > 0 || fir_left_count > 0){
                point_fir.push_back(overlap->get_bounding_rect().get_left_lower());
                if(fir_row_count > 0){
                    point_fir.push_back(vec2d_t(overlap->get_bounding_rect().get_right_upper().get_x(),
                                                overlap->get_bounding_rect().get_left_lower().get_y()));
                    point_fir.push_back(point_fir.back() + vec2d_t(0, fir_row_count));
                }
                if(fir_left_count > 0){
                    point_fir.push_back(vec2d_t(overlap->get_bounding_rect().get_left_lower().get_x() + fir_left_count,
                                                point_fir.back().get_y()));
                    point_fir.push_back(point_fir.back() + vec2d_t(0, 1));
                }
                point_fir.push_back(vec2d_t(overlap->get_bounding_rect().get_left_lower().get_x(),
                                            point_fir.back().get_y()));
                maps[left_bottom->first].add_new_points(line_t::to_lines(point_fir));
            }
            if(sec_row_count > 0 || sec_left_count > 0){
                point_sec.push_back(overlap->get_bounding_rect().get_right_upper());
                if(sec_row_count > 0){
                    point_sec.push_back(vec2d_t(overlap->get_bounding_rect().get_left_lower().get_x(),
                                                overlap->get_bounding_rect().get_right_upper().get_y()));
                    point_sec.push_back(point_sec.back() + vec2d_t(0, -sec_row_count));
                }
                if(sec_left_count > 0){
                    point_sec.push_back(vec2d_t(overlap->get_bounding_rect().get_right_upper().get_x() + sec_left_count,
                                                point_sec.back().get_y()));
                    point_sec.push_back(point_sec.back() + vec2d_t(0, -1));
                }
                point_sec.push_back(vec2d_t(overlap->get_bounding_rect().get_right_upper().get_x(),
                                            point_sec.back().get_y()));
                maps[right_top->first].add_new_points(line_t::to_lines(point_sec));
            }
        }
    }
//    for (auto rect : polygon.get_rects()){
//        if(rect->get_area_from_where().size() == 2) {
//            for(auto area : rect->get_area_from_where()){
//                maps[area.first].minus_new_points(line_t::to_lines({
//                       rect->get_module_bounding_rectangle().getRect().get_left_lower(),
//                       vec2d_t(rect->get_module_bounding_rectangle().getRect().get_right_upper().get_x(),
//                               rect->get_module_bounding_rectangle().getRect().get_left_lower().get_y()),
//                       rect->get_module_bounding_rectangle().getRect().get_right_upper(),
//                       vec2d_t(rect->get_module_bounding_rectangle().getRect().get_left_lower().get_x(),
//                               rect->get_module_bounding_rectangle().getRect().get_right_upper().get_y())
//               }));
//            }
//        }
//    }
    for(auto map : maps){
        auto index = chip_t::get_name_to_index_mapping().at(map.second.module_name);
        if(index >= chip_t::get_soft_modules().size()) continue;
        output_handler_t::utilities.push_back(map.second);
    }
}

void output_handler_t::set_FP(floorplan_t &floorplanning) {
    polygon_forest_t polygonForest = floorplanning.get_polygon_forest();
    polygon_forest_t polygon_carving_forest = carver_t().carving(polygonForest);
    polygon_forest_t polygon_pushing_orest = carver_t().pushing(polygon_carving_forest);
    polygonForest = carver_t().best_pf(polygonForest, polygon_pushing_orest);
    for(auto poly : polygonForest.get_polygons()){
        output_handler_t::parse_polygon(poly);
    }
    for(auto& utility : output_handler_t::utilities){
        utility.bounding_rect = rect_t(vec2d_t(1e9, 1e9), vec2d_t(-1e9, -1e9));
        for(auto point : utility.points){
            if(point.get_x() < utility.bounding_rect.get_left_lower().get_x()){
                utility.bounding_rect = rect_t(vec2d_t(point.get_x(), utility.bounding_rect.get_left_lower().get_y()),
                                               vec2d_t(utility.bounding_rect.get_right_upper().get_x() - point.get_x(), utility.bounding_rect.get_size().get_y()));
            }
            if(point.get_x() > utility.bounding_rect.get_right_upper().get_x()){
                utility.bounding_rect = rect_t(vec2d_t(utility.bounding_rect.get_left_lower().get_x(), utility.bounding_rect.get_left_lower().get_y()),
                                               vec2d_t(point.get_x() - utility.bounding_rect.get_left_lower().get_x(), utility.bounding_rect.get_size().get_y()));
            }
            if(point.get_y() < utility.bounding_rect.get_left_lower().get_y()){
                utility.bounding_rect = rect_t(vec2d_t(utility.bounding_rect.get_left_lower().get_x(), point.get_y()),
                                               vec2d_t(utility.bounding_rect.get_size().get_x(), utility.bounding_rect.get_right_upper().get_y() - point.get_y()));
            }
            if(point.get_y() > utility.bounding_rect.get_right_upper().get_y()){
                utility.bounding_rect = rect_t(vec2d_t(utility.bounding_rect.get_left_lower().get_x(), utility.bounding_rect.get_left_lower().get_y()),
                                               vec2d_t(utility.bounding_rect.get_size().get_x(), point.get_y() - utility.bounding_rect.get_left_lower().get_y()));
            }
        }
    }
    std::vector<vec2d_t> module_position(chip_t::get_modules().size());
    for(auto utility : output_handler_t::utilities){
        module_position[chip_t::get_name_to_index_mapping().at(utility.module_name)] = utility.bounding_rect.get_center();
    }
    for(auto fixed : chip_t::get_fixed_modules()){
        module_position[chip_t::get_name_to_index_mapping().at(fixed->getName())] = fixed->get_left_lower() + (fixed->get_size() / 2);
    }
    double wire_length = 0;
    for (int i = 0; i < module_position.size(); ++i) {
        for (int j = i; j < module_position.size(); ++j) {
            auto dis = module_position[i] - module_position[j];
            wire_length += ((double)chip_t::get_connection_table()[i][j] * (std::fabs(dis.get_x()) + std::fabs(dis.get_y())));
        }
    }
    output_handler_t::hpwl = wire_length;
    std::cout << wire_length << " | output handler\n";
}

double output_handler_t::HPWL() {
    return output_handler_t::hpwl;
}

std::vector<output_utility_t> output_handler_t::to_real() {
    return output_handler_t::utilities;
}

void output_handler_t::test(polygon_forest_t& polygonForest) {
//    polygon_forest_t& polygonForest ;
//    output_handler_t::hpwl = floorplanning.get_wirelength();//float to int?
    for(auto poly : polygonForest.get_polygons()){
        output_handler_t::parse_polygon(poly);
    }
    auto aa = output_handler_t::to_real();
    auto b = aa[0].to_string();
    auto bb = aa[1].to_string();
    std::cout << b << std::endl;
    std::cout << bb << std::endl;
    int i = 0;
    i++;
}

void output_handler_t::ttt() {
//    output_utility_t o1 = output_utility_t("123", {{0,0}});
//    o1.points = {{0,0},{0,0}, {2, 0}, {2, 5}, {0, 5}};
//    o1.reconstruct();

    output_utility_t hhh = output_utility_t("macabaca", {{0,0},{45,0},{45,45},{0,45}});
    hhh.minus_new_points(line_t::to_lines({{ 0,30},{15,30},{15,45},{ 0,45}}));
    visualizer_t::draw_bounding_line({{hhh.points, hhh.module_name}});
    return;


//    output_utility_t hhh = output_utility_t("cpu", {{5,7},{5,2},{8,2},{8,7}});
//    hhh.add_new_points(line_t::to_lines({{5,7},{4,7},{4,6},{5,6}}));
//    visualizer_t::draw_bounding_line({{hhh.points, hhh.module_name}});
//    return;

//    output_utility_t oo = output_utility_t("oo", {{0, 0}, {3, 0}, {3, 2}, {0, 2}});
//    oo.minus_new_points(line_t::to_lines({{0, 0}, {1, 0}, {1, 2}, {0, 2}}));
//    visualizer_t::draw_bounding_line({{oo.points, oo.module_name}});
//    return;
    int i = 0;

    output_utility_t oq = output_utility_t("oq", {{0, 0}, {3, 0}, {3, 2}, {0, 2}});
    oq.minus_new_points(line_t::to_lines({{2, 0}, {3, 0}, {3, 2}, {2, 2}}));
    visualizer_t::draw_bounding_line({{oq.points, oq.module_name}});
    return;


//    output_utility_t o1 = output_utility_t("aa", {{0,0}, {4, 0}, {4, 2}, {2, 2}, {2, 4}, {0, 4}});
//    o1.add_new_points(line_t::to_lines({{2,2}, {5, 2}, {5, 5}, {2, 5}}));
//    o1.minus_new_points(line_t::to_lines({{2,2}, {5, 2}, {5, 5}, {2, 5}}));
//    visualizer_t::draw_bounding_line({{o1.points, o1.module_name}});
//    return;

//    output_utility_t o2 = output_utility_t("aa", {{0,0}, {4, 0}, {4, 2}, {2, 2}, {2, 4}, {0, 4}});
//    o2.add_new_points(line_t::to_lines({{2,2}, {4, 2}, {4, 4}, {2, 4}}));
//    o2.minus_new_points(line_t::to_lines({{2,2}, {4, 2}, {4, 4}, {2, 4}}));
//    visualizer_t::draw_bounding_line({{o2.points, o2.module_name}});
//    return;

//    output_utility_t o3 = output_utility_t("aa", {{0,0}, {4, 0}, {4, 2}, {2, 2}, {2, 4}, {0, 4}});
//    o3.add_new_points(line_t::to_lines({{2,2}, {4, 2}, {4, 5}, {2, 5}}));
//    o3.minus_new_points(line_t::to_lines({{2,2}, {4, 2}, {4, 5}, {2, 5}}));
//    visualizer_t::draw_bounding_line({{o3.points, o3.module_name}});
//    return;
}
