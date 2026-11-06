//
// Created by 林士傑 on 2023/6/20.
//

#ifndef BOUNDINGLINE_BOUNDING_LINE_HANDLER_T_H
#define BOUNDINGLINE_BOUNDING_LINE_HANDLER_T_H

#include <vector>
#include "bounding_line_t.h"

class sequence_pair_t;

class bounding_line_handler_t {
    std::vector<bounding_line_t> bounding_lines;
public:
    bounding_line_handler_t(const vec2d_t& size);

    const std::vector<bounding_line_t>& get_bounding_lines() const;

    void add_bounding_line(const bounding_line_t& bounding_line);

    static bool check_sequence_pair(const sequence_pair_t& sequence_pair);

    //std::vector<rect_t> get_dead_space_each_line();
};


#endif //BOUNDINGLINE_BOUNDING_LINE_HANDLER_T_H
