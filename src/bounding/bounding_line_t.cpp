//
// Created by 林士傑 on 2023/6/19.
//

#include "bounding_line_t.h"
#include <algorithm>
#include <cassert>
#include <cmath>

#pragma region bounding_line_interect_result

bool bounding_line_interect_result_t::operator==(const bounding_line_interect_result_t &bounding_line_interect_result) {
    if(this->difference_pos_line.size() != bounding_line_interect_result.difference_pos_line.size()) return false;
    if(this->difference_neg_line.size() != bounding_line_interect_result.difference_neg_line.size()) return false;
    if(this->intersect_line.size() != bounding_line_interect_result.intersect_line.size()) return false;
    if(this->union_line.size() != bounding_line_interect_result.union_line.size()) return false;
    for(int i = 0; i < this->difference_pos_line.size(); ++i) if(this->difference_pos_line[i] != bounding_line_interect_result.difference_pos_line[i]) return false;
    for(int i = 0; i < this->difference_neg_line.size(); ++i) if(this->difference_neg_line[i] != bounding_line_interect_result.difference_neg_line[i]) return false;
    for(int i = 0; i < this->intersect_line.size(); ++i) if(this->intersect_line[i] != bounding_line_interect_result.intersect_line[i]) return false;
    for(int i = 0; i < this->union_line.size(); ++i) if(this->union_line[i] != bounding_line_interect_result.union_line[i]) return false;
    return true;
}

bool bounding_line_interect_result_t::operator!=(const bounding_line_interect_result_t &bounding_line_interect_result) {
    return !this->operator==(bounding_line_interect_result);
}

#pragma endregion

#pragma region bounding_line_element

bounding_line_element_t::bounding_line_element_t() : line_t() {

}

bounding_line_element_t::bounding_line_element_t(const line_t &line) : line_t(line) {

}

bounding_line_element_t::bounding_line_element_t(const vec2d_t &start, const vec2d_t &end) : line_t(start, end) {

}

bool bounding_line_element_t::operator==(const bounding_line_element_t &bounding_line_element) const {
    return line_t::operator==(bounding_line_element);
}

bool bounding_line_element_t::operator!=(const bounding_line_element_t &bounding_line_element) const {
    return line_t::operator!=(bounding_line_element);
}

#pragma endregion

#pragma region bounding_line_anchor_points

void bounding_line_anchor_points::update_anchor(bounding_node_t* &bounding_node) {
    const vec2d_t& p = bounding_node->get_data().get_start();
    if(p.get_x() < this->anchors[anchor_type::left_lower].get_x() || (p.get_x() == this->anchors[anchor_type::left_lower].get_x() && p.get_y() < this->anchors[anchor_type::left_lower].get_y())) {
        this->anchors[anchor_type::left_lower] = p;
        this->anchor_lines[anchor_type::left_lower] = bounding_node;
    }
    if(p.get_x() < this->anchors[anchor_type::left_upper].get_x() || (p.get_x() == this->anchors[anchor_type::left_upper].get_x() && p.get_y() > this->anchors[anchor_type::left_upper].get_y())) {
        this->anchors[anchor_type::left_upper] = p;
        this->anchor_lines[anchor_type::left_upper] = bounding_node;
    }
    if(p.get_y() > this->anchors[anchor_type::upper_left].get_y() || (p.get_y() == this->anchors[anchor_type::upper_left].get_y() && p.get_x() < this->anchors[anchor_type::upper_left].get_x())) {
        this->anchors[anchor_type::upper_left] = p;
        this->anchor_lines[anchor_type::upper_left] = bounding_node;
    }
    if(p.get_y() > this->anchors[anchor_type::upper_right].get_y() || (p.get_y() == this->anchors[anchor_type::upper_right].get_y() && p.get_x() > this->anchors[anchor_type::upper_right].get_x())) {
        this->anchors[anchor_type::upper_right] = p;
        this->anchor_lines[anchor_type::upper_right] = bounding_node;
    }
    if(p.get_x() > this->anchors[anchor_type::right_upper].get_x() || (p.get_x() == this->anchors[anchor_type::right_upper].get_x() && p.get_y() > this->anchors[anchor_type::right_upper].get_y())) {
        this->anchors[anchor_type::right_upper] = p;
        this->anchor_lines[anchor_type::right_upper] = bounding_node;
    }
    if(p.get_x() > this->anchors[anchor_type::right_lower].get_x() || (p.get_x() == this->anchors[anchor_type::right_lower].get_x() && p.get_y() < this->anchors[anchor_type::right_lower].get_y())) {
        this->anchors[anchor_type::right_lower] = p;
        this->anchor_lines[anchor_type::right_lower] = bounding_node;
    }
    if(p.get_y() < this->anchors[anchor_type::lower_right].get_y() || (p.get_y() == this->anchors[anchor_type::lower_right].get_y() && p.get_x() > this->anchors[anchor_type::lower_right].get_x())) {
        this->anchors[anchor_type::lower_right] = p;
        this->anchor_lines[anchor_type::lower_right] = bounding_node;
    }
    if(p.get_y() < this->anchors[anchor_type::lower_left].get_y() || (p.get_y() == this->anchors[anchor_type::lower_left].get_y() && p.get_x() < this->anchors[anchor_type::lower_left].get_x())) {
        this->anchors[anchor_type::lower_left] = p;
        this->anchor_lines[anchor_type::lower_left] = bounding_node;
    }
}

void bounding_line_anchor_points::set_anchor_line(bounding_line_anchor_points::anchor_type anchor, bounding_node_t* &bounding_node) {
    this->anchor_lines[anchor] = bounding_node;
    this->anchors[anchor] = bounding_node->get_data().get_start();
}

const bounding_node_t* bounding_line_anchor_points::get_anchor_line(bounding_line_anchor_points::anchor_type anchor) const {
    return this->anchor_lines[anchor];
}

const std::vector<bounding_node_t*> bounding_line_anchor_points::get_anchor_lines() const {
    return this->anchor_lines;
}

const vec2d_t &bounding_line_anchor_points::get_anchor_point(bounding_line_anchor_points::anchor_type anchor) const {
    return this->anchors[anchor];
}

#pragma endregion

#pragma region bounding_line

#pragma region basic

bounding_line_t::bounding_line_t() {
    this->lines = circular_T_list_t<bounding_line_element_t>();
}

bounding_line_t::bounding_line_t(circular_T_list_t<bounding_line_element_t> circular_list, bool clockwise) : clockwise(clockwise) {
    this->lines = circular_list;
}

bounding_line_t::bounding_line_t(const std::vector<bounding_line_element_t>& lines, bool clockwise) : clockwise(clockwise) {
    for(const auto& line: lines){
        this->lines.add_at_tail(line);
    }
    this->update();
}

bounding_line_t::bounding_line_t(const rect_t &rect, bool clockwise) : bounding_line_t(rect.to_bounding_point(), clockwise) {
    
}

bounding_line_t::bounding_line_t(const std::vector<vec2d_t>& points, bool clockwise) : clockwise(clockwise) {
    assert(points.size() > 1);
    if(clockwise){
        if(points.front() != points.back()) this->lines.add_at_tail(bounding_line_element_t(points.back(), points.front()));
        for(int i = 1; i < points.size(); ++i) {
            if(points[i - 1] == points[i]) continue;
            this->lines.add_at_tail(bounding_line_element_t(points[i - 1], points[i]));
        }
    }else{
        if(points.front() != points.back()) this->lines.add_at_head(bounding_line_element_t(points.front(), points.back()));
        for(int i = 1; i < points.size(); ++i) {
            if(points[i - 1] == points[i]) continue;
            this->lines.add_at_head(bounding_line_element_t(points[i], points[i - 1]));
        }
    }
    this->update();
}

bounding_line_t bounding_line_t::operator=(const bounding_line_t &bounding_line) {
    this->clockwise = bounding_line.clockwise;
    this->area = bounding_line.area;
    this->lines = bounding_line.lines;
    return *this;
}

bool bounding_line_t::operator==(const bounding_line_t &bounding_line) {
    if(this->get_clockwise() != bounding_line.get_clockwise()) return false;
    if(this->get_area() != bounding_line.get_area()) return false;
    if(this->get_bounding_rect() != bounding_line.get_bounding_rect()) return false;
    if(this->lines != bounding_line.lines) return false;
    return true;
}

bool bounding_line_t::operator!=(const bounding_line_t &bounding_line) {
    return !this->operator==(bounding_line);
}

#pragma endregion

#pragma region basic_func

void bounding_line_t::print() const {
    this->lines.print();
}

void bounding_line_t::print_reverse() const {
    this->lines.print_reverse();
}

void bounding_line_t::plot(std::string plot_name) const {
    visualizer_t::polygon_detail(this->get_nodes(), plot_name);
}

std::ostream &operator<<(std::ostream &os, const bounding_line_t &bounding_line) {
    for(auto vec2d : bounding_line.get_nodes()){
        os << vec2d << ", ";
    }
    return os;
}

#pragma endregion

#pragma region updating

// Merges consecutive collinear segments.
// NOTE: degenerate outlines (e.g. spikes A->B->A) can collapse to a single
// segment. The guards below keep the list consistent in that case: without
// them the wrap-around merge would merge a node with itself, delete it twice
// and finally unlink the tail sentinel (hang / crash).
void bounding_line_t::reduce_line() {
    circular_T_node_t<bounding_line_element_t>* cur = this->lines.begin();
    std::optional<line_t> line = std::nullopt;
    while(cur != this->lines.get_tail() && cur->get_next() != this->lines.get_tail()) {
        if((line = cur->get_data().merge(cur->get_next()->get_data()))) {
            this->lines.delete_node(cur->get_next());
            cur = this->lines.delete_node_get_prev(cur);
            this->lines.concat_next(cur, bounding_line_element_t(line.value()));
            line = std::nullopt;
        }
        cur = cur->get_next();
    }
    // The last and the first segment must be two different nodes.
    if(this->lines.begin() == this->lines.get_tail() ||
       this->lines.begin() == this->lines.get_tail()->get_prev()) {
        return;
    }
    if((line = this->lines.get_tail()->get_prev()->get_data().merge(this->lines.begin()->get_data()))) {
        this->lines.delete_node(this->lines.get_tail()->get_prev());
        this->lines.delete_node(this->lines.begin());
        this->lines.add_at_head(line.value());
    }
}

void bounding_line_t::update_area() {
    double area = 0;
    circular_T_node_t<bounding_line_element_t>* cur = this->lines.get_head();
    while(cur = cur->get_next(), cur != this->lines.get_tail()) {
        area += cur->get_data().ori_dot_area();
    }
    this->area = area / 2.0;
}

void bounding_line_t::update_clockwise() {
    if(area < 0) {
        this->clockwise = true;
    } else {
        this->clockwise = false;
    }
}

void bounding_line_t::update_bounding() {
    circular_T_node_t<bounding_line_element_t>* cur = this->lines.begin();
    vec2d_t left_lower(1e9);
    vec2d_t right_upper(-1e9);
    while(cur != this->lines.get_tail()) {
        left_lower.set_x(std::min(left_lower.get_x(), cur->get_data().get_little_x()));
        left_lower.set_y(std::min(left_lower.get_y(), cur->get_data().get_little_y()));
        right_upper.set_x(std::max(right_upper.get_x(), cur->get_data().get_large_x()));
        right_upper.set_y(std::max(right_upper.get_y(), cur->get_data().get_large_y()));
        cur = cur->get_next();
    }
    this->bounding_rect = rect_t(left_lower, right_upper - left_lower);
}

void bounding_line_t::update_root() {
    circular_T_node_t<bounding_line_element_t>* lower_left = this->lines.begin();
    circular_T_node_t<bounding_line_element_t>* cur = this->lines.get_head();
    while(cur = cur->get_next(), cur != this->lines.get_tail()) {
        if(cur->get_data().get_start().get_y() < lower_left->get_data().get_start().get_y()) {
            lower_left = cur;
        } else if (cur->get_data().get_start().get_y() == lower_left->get_data().get_start().get_y()) {
            if(cur->get_data().get_start().get_x() < lower_left->get_data().get_start().get_x()) {
                lower_left = cur;
            }
        }
    }
    this->lines.drop();
    this->lines.get_tail()->set_prev(lower_left->get_prev());
    lower_left->get_prev()->set_next(this->lines.get_tail());
    lower_left->set_prev(this->lines.get_head());
    this->lines.get_head()->set_next(lower_left);
}

void bounding_line_t::update() {
    this->reduce_line();
    this->update_area();
    this->update_clockwise();
    this->update_bounding();
    this->update_root();
}

#pragma endregion

#pragma region primary_func

bool bounding_line_t::rough_collision(const bounding_line_t &bounding_line) const {
    return this->get_bounding_rect().is_touch(bounding_line.get_bounding_rect());
}

bool bounding_line_t::collision(const bounding_line_t &bounding_line) const {
    if(!rough_collision(bounding_line)) return false;

    auto it0 = this->lines.get_head();
    while(it0 = it0->get_next(), it0 != this->lines.end()) {
        auto it1 = bounding_line.lines.get_head();
        while(it1 = it1->get_next(), it1 != bounding_line.lines.end()) {
            bounding_line_element_t l0 = it0->get_data();
            bounding_line_element_t l1 = it1->get_data();
            auto collt = l0.is_intersect(l1);
            if(collt == line_t::collision_vertical) {
                return true;
            } else if (collt == line_t::collision_horizontal) {
                if(l0.get_line_direction_type() != l1.get_line_direction_type()) {
                    return true;
                }
            }
        }
    }
    return false;
}

namespace {

// Boolean combination of two rectilinear outlines on a compressed grid.
//
// Every outline is an oriented cycle: positive (clockwise) outlines enclose
// their region with winding number +1, negative (counter-clockwise) ones with
// -1. Summing the winding numbers of both inputs gives, e.g., A \ B for a
// positive A and a negative B (A \ B: +1, A n B: 0, B \ A: -1) or A u B for
// two positive outlines. The result consists of the boundaries of the cells
// with winding >= 1 (traced clockwise) and of the cells with winding <= -1
// (traced counter-clockwise).
class winding_grid_t {
   public:
    winding_grid_t(const bounding_line_t& a, const bounding_line_t& b) {
        for (const bounding_line_t* bd : {&a, &b}) {
            for (auto* n = bd->lines.begin(); n != bd->lines.end(); n = n->get_next()) {
                const vec2d_t& s = n->get_data().get_start();
                const vec2d_t& e = n->get_data().get_end();
                if (s != e) {
                    edges.push_back({s, e});
                }
                xs.push_back(s.get_x());
                xs.push_back(e.get_x());
                ys.push_back(s.get_y());
                ys.push_back(e.get_y());
            }
        }
        unique_sort(xs);
        unique_sort(ys);
        nx = static_cast<int>(xs.size()) - 1;
        ny = static_cast<int>(ys.size()) - 1;
        compute_winding();
    }

    // Outlines of the cells with winding >= 1 (positive) or <= -1.
    std::vector<bounding_line_t> outlines(bool positive) const {
        std::vector<char> inside(std::max(nx, 0) * std::max(ny, 0), 0);
        for (size_t c = 0; c < inside.size(); ++c) {
            inside[c] = positive ? winding[c] >= 1 : winding[c] <= -1;
        }
        return trace(inside, positive);
    }

   private:
    struct edge_t {
        vec2d_t s, e;
    };
    // Unit edge between two grid vertices (vertex id = j * (nx + 1) + i).
    struct grid_edge_t {
        int from, to, dir;  // dir: 0 east, 1 north, 2 west, 3 south
    };

    static void unique_sort(std::vector<double>& v) {
        std::sort(v.begin(), v.end());
        v.erase(std::unique(v.begin(), v.end()), v.end());
    }
    static int index_of(const std::vector<double>& v, double value) {
        return static_cast<int>(std::lower_bound(v.begin(), v.end(), value) - v.begin());
    }
    int cell(int i, int j) const { return j * nx + i; }
    int vertex(int i, int j) const { return j * (nx + 1) + i; }

    void compute_winding() {
        winding.assign(std::max(nx, 0) * std::max(ny, 0), 0);
        if (nx <= 0 || ny <= 0) {
            return;
        }
        // delta[j][k]: winding contributed by the vertical edges at xs[k] to
        // every cell of row j left of them (downward edges count +1).
        std::vector<int> delta((nx + 1) * ny, 0);
        for (const edge_t& e : edges) {
            if (e.s.get_x() != e.e.get_x()) {
                continue;  // horizontal edges do not cross a horizontal ray
            }
            const int k = index_of(xs, e.s.get_x());
            const int d = e.e.get_y() < e.s.get_y() ? 1 : -1;
            const int j0 = index_of(ys, std::min(e.s.get_y(), e.e.get_y()));
            const int j1 = index_of(ys, std::max(e.s.get_y(), e.e.get_y()));
            for (int j = j0; j < j1; ++j) {
                delta[j * (nx + 1) + k] += d;
            }
        }
        for (int j = 0; j < ny; ++j) {
            int sum = 0;
            for (int i = nx - 1; i >= 0; --i) {
                sum += delta[j * (nx + 1) + i + 1];
                winding[cell(i, j)] = sum;
            }
        }
    }

    std::vector<bounding_line_t> trace(const std::vector<char>& inside,
                                       bool positive) const {
        auto in = [&](int i, int j) {
            return i >= 0 && j >= 0 && i < nx && j < ny && inside[cell(i, j)];
        };
        // Directed boundary edges with the region on their right (positive)
        // or on their left (negative).
        std::vector<grid_edge_t> grid_edges;
        auto add = [&](int i0, int j0, int i1, int j1, int dir) {
            if (positive) {
                grid_edges.push_back({vertex(i0, j0), vertex(i1, j1), dir});
            } else {
                grid_edges.push_back({vertex(i1, j1), vertex(i0, j0), (dir + 2) % 4});
            }
        };
        for (int j = 0; j < ny; ++j) {
            for (int k = 0; k <= nx; ++k) {  // vertical grid lines
                const bool left = in(k - 1, j), right = in(k, j);
                if (right && !left) add(k, j, k, j + 1, 1);
                if (left && !right) add(k, j + 1, k, j, 3);
            }
        }
        for (int j = 0; j <= ny; ++j) {
            for (int i = 0; i < nx; ++i) {  // horizontal grid lines
                const bool below = in(i, j - 1), above = in(i, j);
                if (below && !above) add(i, j, i + 1, j, 0);
                if (above && !below) add(i + 1, j, i, j, 2);
            }
        }

        std::vector<std::vector<int>> outgoing((nx + 1) * (ny + 1));
        for (int id = 0; id < static_cast<int>(grid_edges.size()); ++id) {
            outgoing[grid_edges[id].from].push_back(id);
        }
        // Where two parts of the region touch at a corner, turn towards the
        // region (right for positive, left for negative outlines) so that the
        // parts are traced as separate outlines.
        const int preferred_turn[3] = {positive ? 3 : 1, 0, positive ? 1 : 3};
        std::vector<char> used(grid_edges.size(), 0);
        std::vector<bounding_line_t> result;
        for (int first = 0; first < static_cast<int>(grid_edges.size()); ++first) {
            if (used[first]) {
                continue;
            }
            std::vector<int> cycle;
            for (int cur = first; cur != -1 && !used[cur];) {
                used[cur] = 1;
                cycle.push_back(cur);
                int next = -1;
                for (const int turn : preferred_turn) {
                    for (const int cand : outgoing[grid_edges[cur].to]) {
                        if (!used[cand] &&
                            grid_edges[cand].dir == (grid_edges[cur].dir + turn) % 4) {
                            next = cand;
                            break;
                        }
                    }
                    if (next != -1) break;
                }
                cur = next;
            }
            result.push_back(to_outline(grid_edges, cycle));
        }
        return result;
    }

    vec2d_t point(int vertex_id) const {
        return vec2d_t(xs[vertex_id % (nx + 1)], ys[vertex_id / (nx + 1)]);
    }

    // Joins the unit edges of a cycle into maximal segments.
    bounding_line_t to_outline(const std::vector<grid_edge_t>& grid_edges,
                               const std::vector<int>& cycle) const {
        const int n = static_cast<int>(cycle.size());
        int start = 0;  // the cycle must not start in the middle of a segment
        while (start < n && grid_edges[cycle[start]].dir ==
                                grid_edges[cycle[(start + n - 1) % n]].dir) {
            ++start;
        }
        std::vector<bounding_line_element_t> segments;
        for (int k = 0; k < n;) {
            const grid_edge_t& e = grid_edges[cycle[(start + k) % n]];
            int end = e.to;
            ++k;
            while (k < n && grid_edges[cycle[(start + k) % n]].dir == e.dir) {
                end = grid_edges[cycle[(start + k) % n]].to;
                ++k;
            }
            segments.emplace_back(point(e.from), point(end));
        }
        return bounding_line_t(segments);
    }

    std::vector<double> xs, ys;
    std::vector<edge_t> edges;
    int nx = 0, ny = 0;
    std::vector<int> winding;  // per cell
};

}  // namespace

bounding_line_interect_result_t bounding_line_t::merge(bounding_line_t bounding_line0, bounding_line_t bounding_line1) {
    if(!bounding_line0.collision(bounding_line1)) {
        bounding_line0.update();
        bounding_line1.update();
        return {{}, {}, {bounding_line0}, {bounding_line1}};
    }
    // Every resulting outline is classified by its orientation (holes of a
    // positive region are negative and vice versa); larger outlines first.
    const winding_grid_t grid(bounding_line0, bounding_line1);
    bounding_line_interect_result_t result;
    for (const bool positive : {true, false}) {
        for (auto& bd : grid.outlines(positive)) {
            (bd.get_clockwise() ? result.difference_pos_line
                                : result.difference_neg_line)
                .push_back(bd);
        }
    }
    auto by_area = [](const bounding_line_t& a, const bounding_line_t& b) {
        return std::abs(a.get_area()) > std::abs(b.get_area());
    };
    std::stable_sort(result.difference_pos_line.begin(), result.difference_pos_line.end(), by_area);
    std::stable_sort(result.difference_neg_line.begin(), result.difference_neg_line.end(), by_area);
    return result;
}

#pragma endregion

#pragma region secondary_func

bool bounding_line_t::vaild_for_80percent() const {
    return this->get_area() * 5 >= this->get_bounding_rect().get_area();
}

int bounding_line_t::get_edge_count() const {
    int edge_count = 0;
    circular_T_node_t<bounding_line_element_t>* it = this->lines.get_head();
    while(it = it->get_next(), it != this->lines.get_tail()) {
        edge_count++;
    }
    return edge_count;
}

int bounding_line_t::get_vertex_count() const {
    int vertex_count = 0;
    circular_T_node_t<bounding_line_element_t>* it = this->lines.get_head();
    while(it = it->get_next(), it != this->lines.get_tail()) {
        vertex_count++;
    }
    return vertex_count;
}

int bounding_line_t::get_vertex270_count() const {
    int vertex270_count = 0;
    circular_T_node_t<bounding_line_element_t>* it = this->lines.get_head();
    while(it = it->get_next(), it->get_next() != this->lines.get_tail()) {
        if(it->get_data().get_line_turn_direction_type(it->get_next()->get_data()) == line_t::line_turn_direction_type::turn_direction_type_left) {
            vertex270_count++;
        }
    }
    if(this->lines.get_tail()->get_prev()->get_data().get_line_turn_direction_type(this->lines.begin()->get_data()) == line_t::line_turn_direction_type::turn_direction_type_left) {
        vertex270_count++;
    }
    return vertex270_count;
}

void bounding_line_t::get_anchor() const {
    bounding_line_anchor_points bdap;
    bounding_node_t* cur = this->lines.get_head();
    while(cur = cur->get_next(), cur != this->lines.get_tail()) {
        bdap.update_anchor(cur);
    }
}

bool bounding_line_t::check_rounded_rect() const {
    double shape_limit = 1 / 3.0;
    bounding_line_anchor_points bdap;
    bounding_node_t* cur = this->lines.get_head();
    while(cur = cur->get_next(), cur != this->lines.get_tail()) {
        bdap.update_anchor(cur);
    }
    for(int i = 0; i < 4; ++i) {
        bounding_node_t* start = bdap.get_anchor_lines()[i * 2];
        bounding_node_t* end = bdap.get_anchor_lines()[i * 2 + 1];
        for(auto s = start; s != end; s = s->get_next()) {
            bounding_node_t* prev = this->lines.get_prev(s);
            bounding_node_t* next = this->lines.get_next(s);
            auto type0 = prev->get_data().get_line_turn_direction_type(s->get_data());
            auto type1 = s->get_data().get_line_turn_direction_type(next->get_data());
            if(type0 != type1 || type0 != line_t::line_turn_direction_type::turn_direction_type_right) continue;
            double ratio = s->get_data().get_vec().get_length();
            if(s->get_data().dot(prev->get_data()) < next->get_data().dot(s->get_data())) {
                ratio /= prev->get_data().get_vec().get_length();
            } else {
                ratio /= next->get_data().get_vec().get_length();
            }
            if(ratio < shape_limit) {
                return false;
            }
        }
    }
    return true;
}

bool bounding_line_t::erode_vaild(const vec2d_t &length) const {
    double width_reject_rate = length.get_x();
    double height_reject_rate = length.get_y();
    double reject_length = std::min(this->get_bounding_rect().get_size().get_x() * width_reject_rate, this->get_bounding_rect().get_size().get_y() * height_reject_rate);
    bounding_line_t bd = *this;
    std::vector<bounding_line_t> erodes;
    bounding_node_t* cur = this->lines.get_head();
    while(cur = cur->get_next(), cur != this->lines.get_tail()) {
        line_t new_line = line_t::normal_line(line_t::turn(cur->get_data().get_line_direction_type(), line_t::turn_direction_type_right));
        vec2d_t left_lower = cur->get_data().get_start();
        vec2d_t new_point = cur->get_data().get_vec() + new_line.get_vec() * reject_length;
        if(new_point.get_x() < 0) {
            left_lower.set_x(left_lower.get_x() + new_point.get_x());
            new_point.set_x(new_point.get_x() * -1);
        }
        if(new_point.get_y() < 0) {
            left_lower.set_y(left_lower.get_y() + new_point.get_y());
            new_point.set_y(new_point.get_y() * -1);
        }
        rect_t new_rect = rect_t(left_lower, new_point);
        erodes.push_back(bounding_line_t(new_rect, false));

        bounding_node_t* next = this->lines.get_next(cur);
        if(cur->get_data().get_line_turn_direction_type(next->get_data()) != line_t::turn_direction_type_left) {
            continue;
        }
        new_line = line_t::normal_line(line_t::turn(cur->get_data().get_line_direction_type(), line_t::turn_direction_type_right));
        left_lower = cur->get_data().get_end();
        new_point = (line_t::normal_line(cur->get_data().get_line_direction_type()).get_vec() + new_line.get_vec()) * reject_length;
        if(new_point.get_x() < 0) {
            left_lower.set_x(left_lower.get_x() + new_point.get_x());
            new_point.set_x(new_point.get_x() * -1);
        }
        if(new_point.get_y() < 0) {
            left_lower.set_y(left_lower.get_y() + new_point.get_y());
            new_point.set_y(new_point.get_y() * -1);
        }
        new_rect = rect_t(left_lower, new_point);
        erodes.push_back(bounding_line_t(new_rect, false));
    }
    bounding_line_t erode = *this;
    for(auto bd : erodes) {
        auto mer = bounding_line_t::merge(erode, bd);
        if(mer.difference_pos_line.size() == 0) return true;
        if(mer.difference_pos_line.size() > 1) return false;
        erode = mer.difference_pos_line[0];
    }
    // std::vector<bounding_line_t> eroodes;
    // eroodes.push_back(*this);
    // for(auto bd : erodes) {
    //     for(auto& er : eroodes) {
    //         auto mer = bounding_line_t::merge(er, bd);
    //         if(mer.difference_pos_line.size() == 0) return true;
    //         er = mer.difference_pos_line[0];
    //         if(mer.difference_pos_line.size() > 1)
    //             eroodes.push_back(mer.difference_pos_line[1]);
    //     }
    // }
    // std::vector<std::pair<std::vector<vec2d_t>, std::string>> tmp;
    // tmp.push_back({this->get_nodes(), "origin"});
    // for(auto er : eroodes) 
    //     tmp.push_back({er.get_nodes(), "erode"});
    // visualizer_t::draw_bounding_line(tmp);
    return true;
}

#pragma endregion

#pragma region getter

std::vector<vec2d_t> bounding_line_t::get_nodes() const {
    std::vector<vec2d_t> vec;
    for(const auto& l : this->lines.to_vector()){
        vec.push_back(l.get_start());
    }
    return vec;
}

double bounding_line_t::get_area() const {
    return std::abs(this->area);
}

bool bounding_line_t::get_clockwise() const {
    return this->clockwise;
}

const rect_t bounding_line_t::get_bounding_rect() const {
    return this->bounding_rect;
}

#pragma endregion

#pragma endregion