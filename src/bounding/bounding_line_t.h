//
// Created by 林士傑 on 2023/6/19.
//

#ifndef BOUNDINGLINE_BOUNDING_LINE_T_H
#define BOUNDINGLINE_BOUNDING_LINE_T_H

#include "circular_T_list_t.h"
#include "utilities/rect_t.h"
#include "utilities/line_t.h"
#include "plugin/visualizer_t.h"

class bounding_line_t;

class bounding_line_interect_result_t {
public:
    std::vector<bounding_line_t> union_line;
    std::vector<bounding_line_t> intersect_line;
    std::vector<bounding_line_t> difference_pos_line;
    std::vector<bounding_line_t> difference_neg_line;
    bounding_line_interect_result_t() {};
    bounding_line_interect_result_t(std::vector<bounding_line_t> union_line, std::vector<bounding_line_t> intersect_line, std::vector<bounding_line_t> difference_pos_line, std::vector<bounding_line_t> difference_neg_line) :
        union_line(union_line), intersect_line(intersect_line), difference_pos_line(difference_pos_line), difference_neg_line(difference_neg_line) {};

    bool operator==(const bounding_line_interect_result_t& bounding_line_interect_result);
    bool operator!=(const bounding_line_interect_result_t& bounding_line_interect_result);
};

class bounding_line_element_t : public line_t {
public:
    bounding_line_element_t();
    bounding_line_element_t(const line_t &line);
    bounding_line_element_t(const vec2d_t &start, const vec2d_t &end);

    bool operator==(const bounding_line_element_t& bounding_line_element) const;

    bool operator!=(const bounding_line_element_t& bounding_line_element) const;
};

using bounding_node_t = circular_T_node_t<bounding_line_element_t>;

class bounding_line_anchor_points {
public:
    enum anchor_type {
        left_lower,
        left_upper,
        upper_left,
        upper_right,
        right_upper,
        right_lower,
        lower_right,
        lower_left
    };
    std::vector<vec2d_t> anchors;
    std::vector<bounding_node_t*> anchor_lines;

    bounding_line_anchor_points() : anchor_lines(std::vector<bounding_node_t*>(8)) {
        this->anchors = {
            {1e9, 1e9},
            {1e9, -1e9},
            {1e9, -1e9},
            {-1e9, -1e9},
            {-1e9, -1e9},
            {-1e9, 1e9},
            {-1e9, 1e9},
            {1e9, 1e9}
        };
    }

    void update_anchor(bounding_node_t*& bounding_node);

    void set_anchor_line(bounding_line_anchor_points::anchor_type anchor, bounding_node_t*& bounding_node);

    const bounding_node_t* get_anchor_line(bounding_line_anchor_points::anchor_type anchor) const;
    
    const std::vector<bounding_node_t*> get_anchor_lines() const;

    const vec2d_t& get_anchor_point(bounding_line_anchor_points::anchor_type anchor) const;
};

class bounding_line_t {
    bool clockwise = false;

    double area;

    rect_t bounding_rect;
    
    bounding_line_t();

    void reduce_line();

    void update_area();

    void update_clockwise();

    void update_bounding();

    void update_root();

    // bool collision(const bounding_line_t& bounding_line) const;
public:
    circular_T_list_t<bounding_line_element_t> lines;
    bounding_line_t(circular_T_list_t<bounding_line_element_t> circular_list, bool clockwise = true);
    bounding_line_t(const std::vector<bounding_line_element_t>& lines, bool clockwise = true);
    bounding_line_t(const rect_t& rect, bool clockwise = true);
    explicit bounding_line_t(const std::vector<vec2d_t>& points, bool clockwise = true);

    bounding_line_t operator=(const bounding_line_t& bounding_line);
    bool operator==(const bounding_line_t& bounding_line);
    bool operator!=(const bounding_line_t& bounding_line);

    void update();

    void print() const;

    void print_reverse() const;

    void plot(std::string plot_name = "") const;

    bool rough_collision(const bounding_line_t& bounding_line) const;

    bool collision(const bounding_line_t& bounding_line) const;

    static bounding_line_interect_result_t merge(bounding_line_t bounding_line0, bounding_line_t bounding_line1);

    bool vaild_for_80percent() const;

    int get_edge_count() const;

    int get_vertex_count() const;

    int get_vertex270_count() const;

    void get_anchor() const;

    bool check_rounded_rect() const;

    bool erode_vaild(const vec2d_t& length) const;

    std::vector<vec2d_t> get_nodes() const;

    double get_area() const;

    bool get_clockwise() const;

    const rect_t get_bounding_rect() const;
};

std::ostream& operator<<(std::ostream& os, const bounding_line_t& bounding_line);

#endif //BOUNDINGLINE_BOUNDING_LINE_T_H
