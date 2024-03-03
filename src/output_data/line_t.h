//
// Created by 林士傑 on 2023/6/17.
//

#ifndef ICCAD2023PD_LINE_T_H
#define ICCAD2023PD_LINE_T_H

#include "utilities/vec2d_t.h"
#include <vector>

class line_t {
public:
    enum line_type {
        horizontal = 0,
        vertical = 1,
        type_none = 2
    };
    enum line_direction{
        north = 0,
        east = 1,
        south = 2,
        west = 3,
        dire_none = 4
    };
private:
    line_type type;
    line_direction direction;
    vec2d_t start;
    vec2d_t end;
public:
    line_t(const vec2d_t &start, const vec2d_t &anEnd);

    static std::vector<line_t> to_lines(const std::vector<vec2d_t> points);

    line_type getType() const;

    const vec2d_t &getStart() const;

    void setStart(const vec2d_t &start);

    const vec2d_t &getAnEnd() const;

    void setAnEnd(const vec2d_t &anEnd);

    bool overlap(const line_t line) const;

};


#endif //ICCAD2023PD_LINE_T_H
