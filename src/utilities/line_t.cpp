//
// Created by 林士傑 on 2023/6/19.
//

#include "line_t.h"

line_t::line_t() {

}

line_t::line_t(const vec2d_t &start, const vec2d_t &end) : _start(start), _end(end) {
    if(this->_start.get_x() == this->_end.get_x()){
        if(this->_start.get_y() < this->_end.get_y()){
            this->_line_type = line_t::line_type::type_vertical;
            this->_line_direction_type = line_t::line_direction_type::direction_type_north;
        }else if(this->_start.get_y() > this->_end.get_y()){
            this->_line_type = line_t::line_type::type_vertical;
            this->_line_direction_type = line_t::line_direction_type::direction_type_south;
        }
    }else if(this->_start.get_y() == this->_end.get_y()){
        if(this->_start.get_x() < this->_end.get_x()){
            this->_line_type = line_t::line_type::type_horizontal;
            this->_line_direction_type = line_t::line_direction_type::direction_type_east;
        }else if(this->_start.get_x() > this->_end.get_x()){
            this->_line_type = line_t::line_type::type_horizontal;
            this->_line_direction_type = line_t::line_direction_type::direction_type_west;
        }
    }
}

bool line_t::operator==(const line_t &line) const {
    return this->get_start() == line.get_start() && this->get_end() == line.get_end();
}

bool line_t::operator!=(const line_t &line) const {
    return this->get_start() != line.get_start() || this->get_end() != line.get_end();
}

line_t line_t::turn(line_turn_direction_type turn_direction) const {
    switch(turn_direction) {
        case line_turn_direction_type::turn_direction_type_straight:
            return *this;
        case line_turn_direction_type::turn_direction_type_left:
            return line_t(this->get_start(), this->get_start() + this->get_vec().turn_right90());
        case line_turn_direction_type::turn_direction_type_around:
            return line_t(this->get_start(), this->get_start() + this->get_vec().turn_right90());
        case line_turn_direction_type::turn_direction_type_right:
            return line_t(this->get_start(), this->get_start() - this->get_vec());
        case line_turn_direction_type::turn_direction_type_none:
            return line_t();
    }
    return line_t();
}

line_t::line_direction_type line_t::turn(line_t::line_direction_type line_direction, line_t::line_turn_direction_type turn_direction) {
    switch(turn_direction) {
        case line_turn_direction_type::turn_direction_type_straight:
            return line_direction;
        case line_turn_direction_type::turn_direction_type_left:
            return line_direction_type((line_direction + 1) & 3);
        case line_turn_direction_type::turn_direction_type_around:
            return line_direction_type((line_direction + 2) & 3);
        case line_turn_direction_type::turn_direction_type_right:
            return line_direction_type((line_direction + 3) & 3);
        case line_turn_direction_type::turn_direction_type_none:
            return line_direction_type::direction_type_none;
    }
    return line_direction_type::direction_type_none;
}

line_t line_t::normal_line(line_t::line_direction_type line_direction) {
    switch(line_direction) {
        case line_direction_type::direction_type_east:
            return line_t({0, 0}, {1, 0});
        case line_direction_type::direction_type_north:
            return line_t({0, 0}, {0, 1});
        case line_direction_type::direction_type_west:
            return line_t({0, 0}, {-1, 0});
        case line_direction_type::direction_type_south:
            return line_t({0, 0}, {0, -1});
        case line_direction_type::direction_type_none:
            return line_t();
    }
    return line_t();
}

const vec2d_t &line_t::get_start() const {
    return this->_start;
}

const vec2d_t &line_t::get_end() const {
    return this->_end;
}
const double line_t::get_little_x() const {
    return this->_line_type == line_type::type_vertical ? this->_start.get_x() : (this->_line_direction_type == line_direction_type::direction_type_east ? this->_start.get_x() : this->_end.get_x());
}

const double line_t::get_little_y() const {
    return this->_line_type == line_type::type_horizontal ? this->_start.get_y() : (this->_line_direction_type == line_direction_type::direction_type_north ? this->_start.get_y() : this->_end.get_y());
}

const double line_t::get_large_x() const {
    return this->_line_type == line_type::type_vertical ? this->_start.get_x() : (this->_line_direction_type == line_direction_type::direction_type_west ? this->_start.get_x() : this->_end.get_x());;
}

const double line_t::get_large_y() const {
    return this->_line_type == line_type::type_horizontal ? this->_start.get_y() : (this->_line_direction_type == line_direction_type::direction_type_south ? this->_start.get_y() : this->_end.get_y());
}

line_t::line_type line_t::get_line_type() const {
    return _line_type;
}

line_t::line_direction_type line_t::get_line_direction_type() const {
    return _line_direction_type;
}

line_t::line_turn_direction_type line_t::get_line_turn_direction_type(const line_t &line) const {
    if(this->get_end() != line.get_start()) return line_turn_direction_type::turn_direction_type_none;
    if(this->get_line_type() == line_type::type_none || line.get_line_type() == line_type::type_none) return line_turn_direction_type::turn_direction_type_none;
    return line_t::line_turn_direction_type((line.get_line_direction_type() - this->get_line_direction_type() + 4) & 3);
}

vec2d_t line_t::get_vec() const {
    return this->_end - this->_start;
}

double line_t::dot(const line_t &line) const {
    auto vec0 = this->get_vec();
    auto vec1 = line.get_vec();
    return vec0.get_x() * vec1.get_y() - vec0.get_y() * vec1.get_x();
}

double line_t::ori_dot_area() const {
    auto vec0 = this->get_start();
    auto vec1 = this->get_vec();
    return vec0.get_x() * vec1.get_y() - vec0.get_y() * vec1.get_x();
}

bool line_t::is_vertical(const line_t &line) const {
    return this->_line_type != line._line_type;
}

bool line_t::is_parallel(const line_t &line) const {
    return this->_line_type == line._line_type;
}

bool line_t::is_same_line(const line_t &line) const {
    if(is_vertical(line)) return false;
    if(this->get_line_type() == line_t::line_type::type_vertical) return this->get_start().get_x() == line.get_start().get_x();
    return this->get_start().get_y() == line.get_start().get_y();
}

vec2d_t line_t::intersect_point(const line_t &line) const {
    if(this->get_line_type() == line_t::line_type::type_vertical) {
        return {this->get_large_x(), line.get_large_y()};
    }
    return {line.get_large_x(), this->get_large_y()};
}

std::optional<line_t> line_t::merge(const line_t &line) const {
    if(this->get_line_direction_type() != line.get_line_direction_type()) return std::nullopt;
    if(this->get_end() != line.get_start()) return std::nullopt;
    return line_t(this->get_start(), line.get_end());
}

line_t::line_collision_type line_t::is_intersect(const line_t &line) const {
    if(is_vertical(line)){
        if(this->_line_type == line_type::type_vertical){
            return (
                this->get_start().get_x() < line.get_large_x() &&
                this->get_start().get_x() > line.get_little_x() &&
                this->get_large_y() > line.get_start().get_y() &&
                this->get_little_y() < line.get_start().get_y())
                ? line_t::line_collision_type::collision_vertical
                : line_t::line_collision_type::collision_type_none;
        }else{
            return (
                this->get_start().get_y() < line.get_large_y() &&
                this->get_start().get_y() > line.get_little_y() &&
                this->get_large_x() > line.get_start().get_x() &&
                this->get_little_x() < line.get_start().get_x())
                ? line_t::line_collision_type::collision_vertical
                : line_t::line_collision_type::collision_type_none;
        }
    }else{
        if(this->_line_type == line_type::type_vertical){
            return (
                (this->get_start().get_x() == line.get_start().get_x()) &&
                this->get_large_y() > line.get_little_y() &&
                line.get_large_y() > this->get_little_y())
                ? line_t::line_collision_type::collision_horizontal
                : line_t::line_collision_type::collision_type_none;
        }else{
            return (
                (this->get_start().get_y() == line.get_start().get_y()) &&
                this->get_large_x() > line.get_little_x() &&
                line.get_large_x() > this->get_little_x())
                ? line_t::line_collision_type::collision_horizontal
                : line_t::line_collision_type::collision_type_none;
        }
    }
}

line_t::line_collision_type line_t::is_touch(const line_t &line) const {
    if(is_vertical(line)){
        if(this->_line_type == line_type::type_vertical){
            return (
                this->get_start().get_x() <= line.get_large_x() &&
                this->get_start().get_x() >= line.get_little_x() &&
                this->get_large_y() >= line.get_start().get_y() &&
                this->get_little_y() <= line.get_start().get_y())
                ? line_t::line_collision_type::collision_vertical
                : line_t::line_collision_type::collision_type_none;
        }else{
            return (
                this->get_start().get_y() <= line.get_large_y() &&
                this->get_start().get_y() >= line.get_little_y() &&
                this->get_large_x() >= line.get_start().get_x() &&
                this->get_little_x() <= line.get_start().get_x())
                ? line_t::line_collision_type::collision_vertical
                : line_t::line_collision_type::collision_type_none;
        }
    }else{
        if(this->_line_type == line_type::type_vertical){
            return (
                (this->get_start().get_x() == line.get_start().get_x()) &&
                this->get_large_y() >= line.get_little_y() &&
                line.get_large_y() >= this->get_little_y())
                ? line_t::line_collision_type::collision_horizontal
                : line_t::line_collision_type::collision_type_none;
        }else{
            return (
                (this->get_start().get_y() == line.get_start().get_y()) &&
                this->get_large_x() >= line.get_little_x() &&
                line.get_large_x() >= this->get_little_x())
                ? line_t::line_collision_type::collision_horizontal
                : line_t::line_collision_type::collision_type_none;
        }
    }
}

std::optional<std::vector<line_t>> line_t::parallel_cut(const line_t &line) const {
    if(!this->is_same_line(line)){
        return std::nullopt;
    }
    if(this->is_intersect(line) == line_t::line_collision_type::collision_type_none){
        return std::nullopt;
    }
    double last_step;
    std::vector<line_t> lines;

    switch(this->get_line_direction_type()){
        case direction_type_east:
            last_step = this->get_little_x();
            if(line.get_little_x() > this->get_little_x()){
                lines.push_back(line_t(vec2d_t(last_step, this->get_start().get_y()), vec2d_t(line.get_little_x(), this->get_start().get_y())));
                last_step = line.get_little_x();
            }
            if(line.get_large_x() < this->get_large_x()){
                lines.push_back(line_t(vec2d_t(last_step, this->get_start().get_y()), vec2d_t(line.get_large_x(), this->get_start().get_y())));
                last_step = line.get_large_x();
            }
            lines.push_back(line_t(vec2d_t(last_step, this->get_start().get_y()), vec2d_t(this->get_large_x(), this->get_start().get_y())));
            break;
        case direction_type_north:
            last_step = this->get_little_y();
            if(line.get_little_y() > this->get_little_y()){
                lines.push_back(line_t(vec2d_t(this->get_start().get_x(), last_step), vec2d_t(this->get_start().get_x(), line.get_little_y())));
                last_step = line.get_little_y();
            }
            if(line.get_large_y() < this->get_large_y()){
                lines.push_back(line_t(vec2d_t(this->get_start().get_x(), last_step), vec2d_t(this->get_start().get_x(), line.get_large_y())));
                last_step = line.get_large_y();
            }
            lines.push_back(line_t(vec2d_t(this->get_start().get_x(), last_step), vec2d_t(this->get_start().get_x(), this->get_large_y())));
            break;
        case direction_type_west:
            last_step = this->get_large_x();
            if(line.get_large_x() < this->get_large_x()){
                lines.push_back(line_t(vec2d_t(last_step, this->get_start().get_y()), vec2d_t(line.get_large_x(), this->get_start().get_y())));
                last_step = line.get_large_x();
            }
            if(line.get_little_x() > this->get_little_x()){
                lines.push_back(line_t(vec2d_t(last_step, this->get_start().get_y()), vec2d_t(line.get_little_x(), this->get_start().get_y())));
                last_step = line.get_little_x();
            }
            lines.push_back(line_t(vec2d_t(last_step, this->get_start().get_y()), vec2d_t(this->get_little_x(), this->get_start().get_y())));
            break;
        case direction_type_south:
            last_step = this->get_large_y();
            if(line.get_large_y() < this->get_large_y()){
                lines.push_back(line_t(vec2d_t(this->get_start().get_x(), last_step), vec2d_t(this->get_start().get_x(), line.get_large_y())));
                last_step = line.get_large_y();
            }
            if(line.get_little_y() > this->get_little_y()){
                lines.push_back(line_t(vec2d_t(this->get_start().get_x(), last_step), vec2d_t(this->get_start().get_x(), line.get_little_y())));
                last_step = line.get_little_y();
            }
            lines.push_back(line_t(vec2d_t(this->get_start().get_x(), last_step), vec2d_t(this->get_start().get_x(), this->get_little_y())));
            break;
        default:
            break;
    }
    return lines;
}

std::optional<std::vector<line_t>> line_t::parallel_cut_reverse(const line_t &line) const {
    if(!this->is_same_line(line)){
        return std::nullopt;
    }
    if(this->is_intersect(line) == line_t::line_collision_type::collision_type_none){
        return std::nullopt;
    }
    double last_step;
    std::vector<line_t> lines;

    switch(this->get_line_direction_type()){
        case direction_type_east:
            last_step = this->get_little_x();
            if(line.get_little_x() > this->get_little_x()){
                lines.push_back(line_t(vec2d_t(last_step, this->get_start().get_y()), vec2d_t(line.get_little_x(), this->get_start().get_y())));
                last_step = line.get_little_x();
            }
            if(line.get_large_x() < this->get_large_x()){
                lines.push_back(line_t(vec2d_t(last_step, this->get_start().get_y()), vec2d_t(line.get_large_x(), this->get_start().get_y())));
                last_step = line.get_large_x();
            }
            lines.push_back(line_t(vec2d_t(last_step, this->get_start().get_y()), vec2d_t(this->get_large_x(), this->get_start().get_y())));
            break;
        case direction_type_north:
            last_step = this->get_little_y();
            if(line.get_little_y() > this->get_little_y()){
                lines.push_back(line_t(vec2d_t(this->get_start().get_x(), last_step), vec2d_t(this->get_start().get_x(), line.get_little_y())));
                last_step = line.get_little_y();
            }
            if(line.get_large_y() < this->get_large_y()){
                lines.push_back(line_t(vec2d_t(this->get_start().get_x(), last_step), vec2d_t(this->get_start().get_x(), line.get_large_y())));
                last_step = line.get_large_y();
            }
            lines.push_back(line_t(vec2d_t(this->get_start().get_x(), last_step), vec2d_t(this->get_start().get_x(), this->get_large_y())));
            break;
        case direction_type_west:
            last_step = this->get_large_x();
            if(line.get_large_x() < this->get_large_x()){
                lines.push_back(line_t(vec2d_t(last_step, this->get_start().get_y()), vec2d_t(line.get_large_x(), this->get_start().get_y())));
                last_step = line.get_large_x();
            }
            if(line.get_little_x() > this->get_little_x()){
                lines.push_back(line_t(vec2d_t(last_step, this->get_start().get_y()), vec2d_t(line.get_little_x(), this->get_start().get_y())));
                last_step = line.get_little_x();
            }
            lines.push_back(line_t(vec2d_t(last_step, this->get_start().get_y()), vec2d_t(this->get_little_x(), this->get_start().get_y())));
            break;
        case direction_type_south:
            last_step = this->get_large_y();
            if(line.get_large_y() < this->get_large_y()){
                lines.push_back(line_t(vec2d_t(this->get_start().get_x(), last_step), vec2d_t(this->get_start().get_x(), line.get_large_y())));
                last_step = line.get_large_y();
            }
            if(line.get_little_y() > this->get_little_y()){
                lines.push_back(line_t(vec2d_t(this->get_start().get_x(), last_step), vec2d_t(this->get_start().get_x(), line.get_little_y())));
                last_step = line.get_little_y();
            }
            lines.push_back(line_t(vec2d_t(this->get_start().get_x(), last_step), vec2d_t(this->get_start().get_x(), this->get_little_y())));
            break;
        default:
            break;
    }
    std::reverse(lines.begin(), lines.end());
    return lines;
}

std::ostream &operator<<(std::ostream &os, const line_t &line) {
    os << "{" << line.get_start() << ", " << line.get_end() << "}";
    return os;
}

bool line_t::overlap(const line_t line) const {
    return this->is_intersect(line) != line_collision_type::collision_type_none;
}

std::vector<line_t> line_t::to_lines(const std::vector<vec2d_t> points)
{
    std::vector<line_t> lines;
    lines.reserve(points.size());
    for (int i = 0; i < points.size() - 1; ++i) {
        lines.push_back(line_t(points[i], points[i + 1]));
    }
    lines.push_back(line_t(points.back(), points.front()));
    return lines;
}