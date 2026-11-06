//
// Created by 林士傑 on 2023/6/20.
//

#include "bounding_line_handler_t.h"
#include "bounding_line_t.h"
#include "process/sequence_pair/sequence_pair_t.h"


bounding_line_handler_t::bounding_line_handler_t(const vec2d_t &size)
{
    this->bounding_lines.push_back(bounding_line_t({
        vec2d_t(0, 0),
        vec2d_t(0, size.get_y()),
        size,
        vec2d_t(size.get_x(), 0)
    },false));
}

const std::vector<bounding_line_t>& bounding_line_handler_t::get_bounding_lines() const {
    return this->bounding_lines;
}

void bounding_line_handler_t::add_bounding_line(const bounding_line_t &bounding_line) {
    auto it = this->bounding_lines.begin();
    while(it != this->bounding_lines.end()) {
        if(it->get_clockwise()) {
            it++;
            continue;
        }
        if(!it->collision(bounding_line)) {
            it++;
            continue;
        }
        auto result = bounding_line_t::merge(*it, bounding_line);
        *it = result.difference_pos_line[0];
        it++;
    }
}

bool bounding_line_handler_t::check_sequence_pair(const sequence_pair_t &sequence_pair) {
    int soft = chip_t::get_soft_modules().size();
    for(int i = 0; i < soft; ++i) {
        bounding_line_t bd = bounding_line_t(sequence_pair.bounding_lines[i].first);
        if(!bd.check_rounded_rect()) return false;
    }
    return true;
}
