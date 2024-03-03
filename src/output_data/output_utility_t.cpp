//
// Created by 林士傑 on 2023/3/27.
//

#include <algorithm>
#include "output_utility_t.h"

std::string output_utility_t::to_string() {
    int minX = this->points.front().get_x();
    int minY = INT32_MAX;
    int index = -1;
    for (int i = 0; i < this->points.size(); ++i) {
        if(this->points[i].get_x() < minX){
            minX = this->points[i].get_x();
            index = 1;
        }
    }
    for (int i = 0; i < this->points.size(); ++i) {
        if(this->points[i].get_x() == minX){
            if(this->points[i].get_y() < minY){
                minY = this->points[i].get_y();
                index = 1;
            }
        }
    }
    for(int i = 0; i < index; ++i){
        this->points.push_back(this->points.front());
        this->points.erase(this->points.begin());
    }

    std::reverse(this->points.begin(), this->points.end());

    std::string str = "\n" + this->module_name;
    str += " " + std::to_string(points.size());
    for (const auto& p : points) {
        str += "\n" + std::to_string((int)p.get_x()) + " " + std::to_string((int)p.get_y());
    }
    return str;
}

output_utility_t::output_utility_t(const std::string &moduleName, const std::vector<vec2d_t> &points) :
module_name(moduleName), points(points) {
    this->lines = line_t::to_lines(this->points);
}

void output_utility_t::reconstruct() {
    this->lines = line_t::to_lines(this->points);
}

void output_utility_t::add_new_points(const std::vector<line_t> &lines) {
    if(this->points.empty()){
        for(auto line : lines){
            this->points.push_back(line.getStart());
            this->lines.push_back(line);
        }
        return;
    }
    int _i, _j;
    for (_i = 0; _i < this->lines.size(); ++_i) {
        for (_j = lines.size() - 1; _j >= 0; --_j) {
            if (this->lines[_i].overlap(lines[_j])){
                goto evil;
            }
        }
    }
    evil:
    int stop_i = _i, stop_j = _j;
    while(this->lines[stop_i].overlap(lines[stop_j])){
        stop_i = (stop_i + 1) % this->lines.size();
        stop_j = (stop_j - 1 + lines.size()) % lines.size();
    }
    for (int i = _i + 1; i != stop_i; i = (i + 1) % this->lines.size() ) {
        int delete_place = (_i + 1) % this->lines.size();
        if (delete_place >= this->points.size()) delete_place = 0;
        this->points.erase(this->points.begin() + delete_place);
    }
    if ((stop_j + 1) % lines.size() == _j){
        for(int i = 0; i < lines.size(); ++i){
            auto test = lines[(stop_j + 1 - i + lines.size()) % lines.size()].getStart();
            if(this->points[(_i + 1) % this->lines.size()] == lines[(stop_j + 1 - i + lines.size()) % lines.size()].getStart()){
                this->points.erase(this->points.begin() + (_i + 1) % this->lines.size());
            }else if(this->points[(_i) % this->lines.size()] == lines[(stop_j + 1 - i + lines.size()) % lines.size()].getStart()){
                this->points.erase(this->points.begin() + (_i) % this->lines.size());
            }else{
                this->points.insert(this->points.begin() + (_i + 1) % this->lines.size(), lines[(stop_j + 1 - i + lines.size()) % lines.size()].getStart());
            }
        }
    }else{
        for (int i = (stop_j + 1) % lines.size(); i != _j; i = (i - 1 + lines.size()) % lines.size()) {
            auto first = this->points[(_i + 1) % this->lines.size()];
            auto sec = this->points[(_i) % this->lines.size()];
            auto place = this->points.begin() + (_i + 1) % this->lines.size();
            if(this->points[(_i + 1) % this->lines.size()] == lines[i].getStart()){
                this->points.erase(this->points.begin() + (_i + 1) % this->lines.size());
            }else if(this->points[(_i) % this->lines.size()] == lines[i].getStart()){
                this->points.erase(this->points.begin() + (_i) % this->lines.size());
            }else{
                this->points.insert(this->points.begin() + (_i + 1) % this->lines.size(), lines[i].getStart());
            }
        }
    }
    reconstruct();
    int t = 0;
}

void output_utility_t::minus_new_points(const std::vector<line_t> &lines) {
    int _i, _j;
    for (_i = 0; _i < this->lines.size(); ++_i) {
        for (_j = 0; _j < lines.size(); ++_j) {
            if (this->lines[_i].overlap(lines[_j])){
                goto evil;
            }
        }
    }
    evil:
    int stop_i = _i, stop_j = _j;
    while(this->lines[stop_i].overlap(lines[stop_j])){
        stop_i = (stop_i + 1) % this->lines.size();
        stop_j = (stop_j + 1) % lines.size();
        if(stop_i == _i && stop_j == _j){
            this->points.clear();
            this->lines.clear();
            return;
        }
    }
    while(this->lines[(_i - 1 + this->lines.size()) % this->points.size()].overlap(lines[(_j - 1 + lines.size()) % lines.size()])){
        _i = (_i - 1 + this->lines.size()) % this->points.size();
        _j = (_j - 1 + lines.size()) % lines.size();
//        stop_i = (stop_i + 1) % this->lines.size();
//        stop_j = (stop_j + 1) % lines.size();
    }
    bool delete_flag = false;
    for (int i = (_i + 1) % this->lines.size(); i != stop_i; i = (i + 1) % this->lines.size() ) {
        int delete_place = (_i + 1) % this->lines.size();
        if (delete_place >= this->points.size()){
            delete_flag = true;
            delete_place = 0;
        }
        this->points.erase(this->points.begin() + delete_place);
    }
    if((stop_j) % lines.size() == (_j + 1) % lines.size()){
        for (int i = 0; i < lines.size(); ++i) {
            if(delete_flag){
                this->points.insert(this->points.begin(), lines[(stop_j + i) % lines.size()].getStart());
            }else{
                this->points.insert(this->points.begin() + (_i + 1) % this->lines.size(), lines[(stop_j + i) % lines.size()].getStart());
            }
        }
    }else{
        for (int i = (stop_j) % lines.size(); i != (_j + 1) % lines.size(); i = (i + 1) % lines.size()) {
            auto first = this->points[(_i + 1) % this->lines.size()];
            auto sec = this->points[(_i) % this->lines.size()];
            auto place = this->points.begin() + (_i + 1) % this->lines.size();
            auto rea = lines[i].getStart();
//        if(this->points[(_i + 1) % this->lines.size()] == lines[i].getStart()){
//            this->points.erase(this->points.begin() + (_i + 1) % this->lines.size());
//        }else if(this->points[(_i) % this->lines.size()] == lines[i].getStart()){
//            this->points.erase(this->points.begin() + (_i) % this->lines.size());
//        }else{
            if(delete_flag){
                this->points.insert(this->points.begin(), lines[i].getStart());
            }else{
                this->points.insert(this->points.begin() + (_i + 1) % this->lines.size(), lines[i].getStart());
            }
//        }
        }
    }
    reconstruct();
}

output_utility_t::output_utility_t() {

}
