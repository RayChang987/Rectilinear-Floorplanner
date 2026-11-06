//
// Created by 林士傑 on 2023/3/9.
//

#ifndef INC_2023PD_RECT_H
#define INC_2023PD_RECT_H

#include <vector>
#include "vec2d_t.h"
#include "utilities/box_t.h"

class rect_t : public box_t {
    vec2d_t LeftLower;
    vec2d_t RightUpper;
    vec2d_t Center;
    vec2d_t Size;

public:
    const rect_t &get_bounding_rect() const override;

    rect_t(const vec2d_t &left_lower, const vec2d_t &size);

    rect_t(const rect_t&);

    rect_t() = default;

    const vec2d_t &get_left_lower() const;

    const vec2d_t &get_right_upper() const;

    const vec2d_t &get_center() const;

    const vec2d_t &get_size() const;

    double get_area() const;

    bool is_collision(const rect_t& rect) const;

    bool is_touch(const rect_t& rect) const;

    bool is_contain(const rect_t& rect) const;

    bool is_wrap(const rect_t& rect) const;

    std::vector<rect_t> cut(const rect_t& rect) const;

    std::pair<bool, rect_t> intersect(const rect_t& rect) const;

    const rect_t merge_bounding_rect(const rect_t& rect) const;

    std::vector<vec2d_t> to_bounding_point() const;

    bool operator==(const rect_t& rect) const;

    bool operator!=(const rect_t& rect) const;
};

std::ostream& operator<<(std::ostream& os, const rect_t& vec);

#endif //INC_2023PD_RECT_H
