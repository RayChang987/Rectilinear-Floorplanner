//
// Created by 林士傑 on 2023/6/17.
//

#include "line_t.h"
#include "exception"

line_t::line_t(const vec2d_t &start, const vec2d_t &anEnd) : start(start), end(anEnd), type(line_t::line_type::type_none), direction(line_t::line_direction::dire_none) {
    if(this->start.get_x() == this->end.get_x()){
        this->type = line_t::line_type::vertical;
        if(this->end.get_y() > this->start.get_y()){
            this->direction = line_t::line_direction::north;
        }else{
            this->direction = line_t::line_direction::south;
        }
    }
    else if(this->start.get_y() == this->end.get_y()){
        this->type = line_t::line_type::horizontal;
        if(this->end.get_x() > this->start.get_x()){
            this->direction = line_t::line_direction::east;
        }else{
            this->direction = line_t::line_direction::west;
        }
    }else{
        throw std::exception();
    }
}

bool line_t::overlap(const line_t line) const{
    if(this->type != line.type) return false;
    if(this->type == line_t::line_type::horizontal){
        if(this->start.get_y() != line.start.get_y()) return false;
        if(std::max(this->start.get_x(), this->end.get_x()) > std::min(line.start.get_x(), line.end.get_x()) &&
                std::max(line.start.get_x(), line.end.get_x()) > std::min(this->start.get_x(), this->end.get_x())){
            return true;
        }
    }else{
        if(this->start.get_x() != line.start.get_x()) return false;
        if(std::max(this->start.get_y(), this->end.get_y()) > std::min(line.start.get_y(), line.end.get_y()) &&
           std::max(line.start.get_y(), line.end.get_y()) > std::min(this->start.get_y(), this->end.get_y())){
            return true;
        }
    }
    return false;
}

line_t::line_type line_t::getType() const {
    return type;
}

const vec2d_t &line_t::getStart() const {
    return start;
}

void line_t::setStart(const vec2d_t &start) {
    line_t::start = start;
}

const vec2d_t &line_t::getAnEnd() const {
    return end;
}

void line_t::setAnEnd(const vec2d_t &anEnd) {
    end = anEnd;
}

std::vector<line_t> line_t::to_lines(const std::vector<vec2d_t> points) {
    std::vector<line_t> lines;
    lines.reserve(points.size());
    for (int i = 0; i < points.size() - 1; ++i) {
        lines.push_back(line_t(points[i], points[i + 1]));
    }
    lines.push_back(line_t(points.back(), points.front()));
    return lines;
}