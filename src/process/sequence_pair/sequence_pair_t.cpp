//
// Created by RayChang on 2023/5/6.
//
#include "sequence_pair_t.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "bounding/bounding_line_t.h"
#include "plugin/visualizer_t.h"
#include "process/functional/random_helper.h"
#include "sp_ilp_settings_t.h"
#include "static_data/fp_rule_t.h"
#include "static_data/io_handler_t.h"

int sequence_pair_t::sequence_n;
int sequence_pair_t::fix_start_idx;
int sequence_pair_t::fixed_n;
int sequence_pair_t::soft_n;
int sequence_pair_t::connection_graph_deg;
vector<bool> sequence_pair_t::seq_is_fix;
vector<soft_module_t*> sequence_pair_t::seq_soft_map;
vector<fixed_module_t*> sequence_pair_t::seq_fixed_map;
vector<net_t> sequence_pair_t::connections;
vector<pair<int, int>> sequence_pair_t::deg_w;
vector<double> sequence_pair_t::modules_area;

using std::cout;
using std::endl;
using std::string;

namespace {

// Prints `{a, b, c}`.
template <class T>
void print_list(std::ostream& os, const vector<T>& list) {
    os << "{";
    for (size_t i = 0; i < list.size(); ++i) {
        os << list[i] << (i + 1 != list.size() ? ", " : "");
    }
    os << "}";
}

string module_name(int i) {
    return (sequence_pair_t::seq_is_fix[i] ? "f" : "s") + std::to_string(i);
}

// Transitive reduction of a constraint graph. The sweep below updates the
// adjacency matrix in place (row by row), exactly like the original
// implementation, which keeps the produced LPs unchanged.
void reduce_transitive_edges(vector<edge_t>& edges, int n) {
    vector<char> g(static_cast<size_t>(n) * n, 0);
    auto at = [&](int i, int j) -> char& {
        return g[static_cast<size_t>(i) * n + j];
    };
    for (const edge_t& e : edges) {
        at(e.from, e.to) = 1;
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
                if (at(i, k) && at(k, j)) {
                    at(i, j) = 0;
                    break;
                }
            }
        }
    }
    edges.erase(std::remove_if(edges.begin(), edges.end(),
                               [&](const edge_t& e) { return !at(e.from, e.to); }),
                edges.end());
}

// Transitive reduction for the LP that allows overlaps: an edge may only be
// dropped if it is implied by a path of edges that cannot be relaxed by an
// overlap variable. Otherwise two modules which must not overlap (e.g. of
// similar size) could overlap through a small module in between.
template <class Relaxable>
void reduce_transitive_edges_strict(vector<edge_t>& edges, int n,
                                    Relaxable relaxable) {
    vector<char> strict(static_cast<size_t>(n) * n, 0);
    for (const edge_t& e : edges) {
        if (!relaxable(e)) {
            strict[static_cast<size_t>(e.from) * n + e.to] = 1;
        }
    }
    auto implied = [&](const edge_t& e) {
        for (int k = 0; k < n; ++k) {
            if (strict[static_cast<size_t>(e.from) * n + k] &&
                strict[static_cast<size_t>(k) * n + e.to]) {
                return true;
            }
        }
        return false;
    };
    edges.erase(std::remove_if(edges.begin(), edges.end(), implied), edges.end());
}

}  // namespace

// ---------------------------------------------------------------------------
// Static data
// ---------------------------------------------------------------------------

void sequence_pair_t::init() {
    const auto& soft_modules = chip_t::get_soft_modules();
    const auto& fixed_modules = chip_t::get_fixed_modules();
    fixed_n = static_cast<int>(fixed_modules.size());
    soft_n = static_cast<int>(soft_modules.size());
    sequence_n = static_cast<int>(chip_t::get_total_module_n());
    fix_start_idx = soft_n;
    seq_soft_map.assign(sequence_n, nullptr);
    seq_fixed_map.assign(sequence_n, nullptr);
    seq_is_fix.assign(sequence_n, false);
    modules_area.assign(sequence_n, 0);
    deg_w = vector<pair<int, int>>(sequence_n, {0, 0});

    // Sequence numbers: soft modules first, then fixed modules.
    for (int i = 0; i < soft_n; ++i) {
        seq_soft_map[i] = soft_modules[i];
        modules_area[i] = soft_modules[i]->get_area();
    }
    for (int i = 0; i < fixed_n; ++i) {
        const int j = soft_n + i;
        seq_fixed_map[j] = fixed_modules[i];
        modules_area[j] = fixed_modules[i]->get_area();
        seq_is_fix[j] = true;
    }
    build_graph();
}

void sequence_pair_t::build_graph() {
    connections.clear();
    connection_graph_deg = 0;
    for (int i = 0; i < sequence_n; ++i) {
        deg_w[i].second = i;
    }
    for (multi_net_t* net : chip_t::get_multi_nets()) {
        vector<int> connected_id;
        for (pin_t* pin : net->pins) {
            connected_id.push_back(pin->module_index);
            deg_w[pin->module_index].first += net->weight;
        }
        connection_graph_deg += static_cast<int>(connected_id.size());
        connections.push_back({connected_id, net->weight});
    }
    std::sort(deg_w.begin(), deg_w.end());
    std::reverse(deg_w.begin(), deg_w.end());
}

vector<vec2d_t> sequence_pair_t::find_w_h(uint32_t area, int resolution) {
    if (area == 1) {
        return {{1, 1}};
    }
    const double lower = fp_rule_t::get_lower_ratio();
    const double upper = fp_rule_t::get_upper_ratio();
    // Shorter edges x = p * b^i spread geometrically over the legal ratios.
    const double p = std::sqrt(static_cast<double>(area) * lower);
    const double b = std::pow(upper, 1.0 / static_cast<double>(resolution - 1));
    vector<vec2d_t> res;
    for (int i = 0; i <= resolution - 1; ++i) {
        const double x = p * std::pow(b, i);
        const double y = area / x;
        if (std::ceil(x) * upper >= std::ceil(y)) {
            double xx = std::ceil(x), yy = std::ceil(y);
            if (xx - 1 >= yy * lower && (xx - 1) * yy >= area) {
                xx -= 1;
            }
            if (yy - 1 >= lower * xx && (yy - 1) * xx >= area) {
                yy -= 1;
            }
            res.push_back({xx + 1, yy + 1});
        }
    }
    return res;
}

// ---------------------------------------------------------------------------
// Construction & initialisation
// ---------------------------------------------------------------------------

sequence_pair_t::sequence_pair_t() {
    const auto& soft_modules = chip_t::get_soft_modules();
    soft_area_to_w_h_m_5.resize(soft_modules.size());
    for (size_t i = 0; i < soft_modules.size(); ++i) {
        soft_area_to_w_h_m_5[i] = find_w_h(soft_modules[i]->get_area(), shape_n);
    }
    init_modules_size();
    set_is_in_seq(0);
    set_only_fix();
    set_add_order();
}

void sequence_pair_t::init_modules_size() {
    modules_wh.resize(sequence_n);
    modules_wh_i.resize(soft_n);
    for (int i = 0; i < sequence_n; ++i) {
        if (seq_is_fix[i]) {
            modules_wh[i] = seq_fixed_map[i]->get_size();
        } else {
            modules_wh[i] = soft_area_to_w_h_m_5[i][0];
            modules_wh_i[i] = 0;
        }
    }
}

void sequence_pair_t::set_is_in_seq(int value) {
    is_in_seq.assign(sequence_n, value);
}

void sequence_pair_t::set_only_fix() {
    fix_v_sequence.assign(fixed_n, 0);
    fix_h_sequence.assign(fixed_n, 0);
    set_fix_sequence();
    v_sequence = fix_v_sequence;
    h_sequence = fix_h_sequence;
    for (int i = fix_start_idx; i < sequence_n; ++i) {
        is_in_seq[i] = 1;
    }
}

void sequence_pair_t::set_fix_sequence() {
    if (fixed_n == 0) {
        return;
    }
    // left_of[i][j]: i is entirely left of j, below[i][j]: i is entirely below j
    vector<vector<char>> left_of(sequence_n, vector<char>(sequence_n, 0));
    vector<vector<char>> below(sequence_n, vector<char>(sequence_n, 0));
    for (int i = fix_start_idx; i < sequence_n; ++i) {
        for (int j = fix_start_idx; j < sequence_n; ++j) {
            if (i == j) {
                continue;
            }
            const vec2d_t ll_i = seq_fixed_map[i]->get_left_lower();
            const vec2d_t ll_j = seq_fixed_map[j]->get_left_lower();
            const vec2d_t size_i = seq_fixed_map[i]->get_size();
            const int ix = ll_i.get_x(), iy = ll_i.get_y();
            const int jx = ll_j.get_x(), jy = ll_j.get_y();
            const int iw = size_i.get_x(), ih = size_i.get_y();
            left_of[i][j] = (ix + iw <= jx);
            below[i][j] = (iy + ih <= jy);
        }
    }
    // Insert the fixed modules one by one at the first position consistent
    // with the relations of the modules inserted before.
    vector<int> upd_v, upd_h, v_map(sequence_n), h_map(sequence_n);
    for (int i = fix_start_idx; i < sequence_n; ++i) {
        for (size_t j = 0; j < std::min(upd_v.size(), upd_h.size()); ++j) {
            v_map[upd_v[j]] = h_map[upd_h[j]] = static_cast<int>(j);
        }
        bool inserted = false;
        for (int j = 0; j <= static_cast<int>(upd_v.size()) && !inserted; ++j) {
            for (int k = 0; k <= static_cast<int>(upd_h.size()) && !inserted;
                 ++k) {
                bool ok = true;
                for (int p = fix_start_idx; p < i && ok; ++p) {
                    const bool i_first_v = j <= v_map[p];
                    const bool i_first_h = k <= h_map[p];
                    if (i_first_v && i_first_h) {  // V[i, p], H[i, p]: i -> p
                        ok = left_of[i][p];
                    } else if (!i_first_v && !i_first_h) {  // p -> i
                        ok = left_of[p][i];
                    } else if (i_first_v) {  // V[i, p], H[p, i]: i below p
                        ok = below[i][p];
                    } else {  // V[p, i], H[i, p]: p below i
                        ok = below[p][i];
                    }
                }
                if (ok) {
                    upd_v.insert(upd_v.begin() + j, i);
                    upd_h.insert(upd_h.begin() + k, i);
                    inserted = true;
                }
            }
        }
    }
    fix_v_sequence = upd_v;
    fix_h_sequence = upd_h;
}

void sequence_pair_t::set_add_order() {
    add_soft_order.resize(soft_n);
    for (int i = 0; i < soft_n; ++i) {
        add_soft_order[i] = i;
    }
    // Larger modules first
    std::sort(add_soft_order.begin(), add_soft_order.end(),
              [](int a, int b) { return modules_area[a] > modules_area[b]; });
}

bool sequence_pair_t::is_completed() {
    for (int i = 0; i < sequence_n; ++i) {
        if (is_in_seq[i] == 0) {
            return false;
        }
    }
    return find_position_with_area(false, false);
}

// ---------------------------------------------------------------------------
// get & set
// ---------------------------------------------------------------------------

void sequence_pair_t::set_v(std::vector<int> v) { v_sequence = std::move(v); }
void sequence_pair_t::set_h(std::vector<int> h) { h_sequence = std::move(h); }
std::vector<int> sequence_pair_t::get_v() const { return v_sequence; }
std::vector<int> sequence_pair_t::get_h() const { return h_sequence; }
void sequence_pair_t::set_vi(int i, int x) { v_sequence[i] = x; }
void sequence_pair_t::set_hi(int i, int x) { h_sequence[i] = x; }
int sequence_pair_t::get_vi(int i) const { return v_sequence[i]; }
int sequence_pair_t::get_hi(int i) const { return h_sequence[i]; }
void sequence_pair_t::swap_v(int a, int b) {
    std::swap(v_sequence[a], v_sequence[b]);
}
void sequence_pair_t::swap_h(int a, int b) {
    std::swap(h_sequence[a], h_sequence[b]);
}

void sequence_pair_t::change_size(int i) {
    if (seq_is_fix[i]) {
        modules_wh[i] = seq_fixed_map[i]->get_size();
    } else {
        const int id = static_cast<int>(random_helper::get_rand()) %
                       soft_area_to_w_h_m_5[i].size();
        modules_wh[i] = soft_area_to_w_h_m_5[i][id];
        modules_wh_i[i] = id;
    }
}

void sequence_pair_t::set_module_size(int i, int j) {
    modules_wh[i] = soft_area_to_w_h_m_5[i][j];
    modules_wh_i[i] = j;
}

vector<vec2d_t> sequence_pair_t::get_4_points(const vec2d_t& ll,
                                              const vec2d_t& wh) {
    const vec2d_t ul(ll.get_x(), ll.get_y() + wh.get_y());
    const vec2d_t ur(ll.get_x() + wh.get_x(), ll.get_y() + wh.get_y());
    const vec2d_t lr(ll.get_x() + wh.get_x(), ll.get_y());
    return {ll, ul, ur, lr};
}

// ---------------------------------------------------------------------------
// Constraint graphs
// ---------------------------------------------------------------------------

void sequence_pair_t::build_constraint_graph() {
    constraint_graph_h.clear();
    constraint_graph_v.clear();
    vector<int> v_map(sequence_n), h_map(sequence_n);
    for (size_t i = 0; i < v_sequence.size(); ++i) {
        v_map[v_sequence[i]] = static_cast<int>(i);
        h_map[h_sequence[i]] = static_cast<int>(i);
    }
    for (int i = 0; i < sequence_n; ++i) {
        for (int j = 0; j < sequence_n; ++j) {
            if (is_in_seq[i] == 0 || is_in_seq[j] == 0) {
                continue;
            }
            if (v_map[i] < v_map[j] && h_map[i] < h_map[j]) {
                // V{.., i, .., j, ..}, H{.., i, .., j, ..}: i left of j
                constraint_graph_h.push_back(
                    {i, j, static_cast<int>(modules_wh[i].get_x())});
            } else if (v_map[i] < v_map[j] && h_map[j] < h_map[i]) {
                // V{.., i, .., j, ..}, H{.., j, .., i, ..}: i below j
                constraint_graph_v.push_back(
                    {i, j, static_cast<int>(modules_wh[i].get_y())});
            }
        }
    }
}

void sequence_pair_t::simplify_constraint_graph() {
    reduce_transitive_edges(constraint_graph_v, sequence_n);
    reduce_transitive_edges(constraint_graph_h, sequence_n);
}

// ---------------------------------------------------------------------------
// LP based placement
// ---------------------------------------------------------------------------

void sequence_pair_t::set_shape_layout() {
    // [x | y | shape_0 .. shape_4 | x_e_l | x_e_r | y_e_l | y_e_r | ...]
    const int c = static_cast<int>(connections.size());
    x_module_offset = 1;
    y_module_offset = 1 + sequence_n;
    for (int i = 0; i < shape_n; ++i) {
        shape_types[i] = 1 + 2 * sequence_n + i * soft_n;
    }
    x_edge_offset_l = 1 + 2 * sequence_n + shape_n * soft_n;
    x_edge_offset_r = x_edge_offset_l + c;
    y_edge_offset_l = x_edge_offset_l + 2 * c;
    y_edge_offset_r = x_edge_offset_l + 3 * c;
}

vector<vec2d_t> sequence_pair_t::get_LP_res_pos(
    const ILP_result_t& result) const {
    vector<vec2d_t> pos(sequence_n);
    for (int i = 0; i < sequence_n; ++i) {
        if (is_in_seq[i] == 0) {
            pos[i] = {-1, -1};
        } else {
            pos[i] = {static_cast<int>(result.var_values[i + x_module_offset]),
                      static_cast<int>(result.var_values[i + y_module_offset])};
        }
    }
    return pos;
}

vector<vec2d_t> sequence_pair_t::get_LP_res_wh(
    const ILP_result_t& result) const {
    vector<vec2d_t> wh(soft_n);
    for (int i = 0; i < soft_n; ++i) {
        if (is_in_seq[i] == 0) {
            wh[i] = {-1, -1};
            continue;
        }
        // The shape is the weighted combination of the samples.
        double x = 0, y = 0;
        for (int j = 0; j < shape_n; ++j) {
            const double weight = result.var_values[i + shape_types[j]];
            x += weight * soft_area_to_w_h_m_5[i][j].get_x();
            y += weight * soft_area_to_w_h_m_5[i][j].get_y();
        }
        wh[i] = {std::floor(x), std::floor(y)};
    }
    return wh;
}

void sequence_pair_t::load_LP_result(const ILP_result_t& result) {
    modules_positions = get_LP_res_pos(result);
    const vector<vec2d_t> wh = get_LP_res_wh(result);
    for (int i = 0; i < soft_n; ++i) {
        modules_wh[i] = {static_cast<int>(wh[i].get_x()),
                         static_cast<int>(wh[i].get_y())};
    }
    modules_wh_i.assign(soft_n, -1);  // the shapes are determined by the LP
    lp_predicted_wirelength = result.z;
}

bool sequence_pair_t::find_position(bool minimize_wirelength,
                                    bool load_result) {
    build_constraint_graph();
    simplify_constraint_graph();
    set_shape_layout();
    variable_n = 2 * sequence_n + shape_n * soft_n;
    if (minimize_wirelength) {
        variable_n += 4 * static_cast<int>(connections.size());
    }
    if (!ILP_solver.init(variable_n)) {
        return false;
    }
    ILP_solver.set_min();

    sp_ilp_settings_find_position_t settings(this);
    settings.set_constraints_modules_overlap_h();
    settings.set_constraints_modules_overlap_v();
    settings.set_constraints_modules_fixed();
    if (minimize_wirelength) {
        settings.set_constraints_net_direction();
        settings.set_constraints_net();
    }
    settings.set_constraints_ratio_equal_1();
    settings.set_constraints_boundaries();
    settings.set_variables_modules();
    if (minimize_wirelength) {
        settings.set_variables_connections();
    }
    settings.set_variables_shapes();

    vector<double> coef(variable_n + 1, 0);
    if (minimize_wirelength) {
        settings.set_coef(coef);
    }
    ILP_solver.set_obj_coef(coef);
    const ILP_result_t result = ILP_solver.solve(false);
    if (!result.legal) {
        return false;
    }
    if (load_result) {
        load_LP_result(result);
    }
    return true;
}

bool sequence_pair_t::find_position_with_area(bool minimize_wirelength,
                                              bool load_result) {
    build_constraint_graph();
    // [x | y | x_e_l | x_e_r | y_e_l | y_e_r]
    const int c = static_cast<int>(connections.size());
    x_module_offset = 1;
    y_module_offset = 1 + sequence_n;
    x_edge_offset_l = 1 + 2 * sequence_n;
    x_edge_offset_r = x_edge_offset_l + c;
    y_edge_offset_l = x_edge_offset_l + 2 * c;
    y_edge_offset_r = x_edge_offset_l + 3 * c;
    variable_n = 2 * sequence_n + (minimize_wirelength ? 4 * c : 0);
    if (!ILP_solver.init(variable_n)) {
        return false;
    }
    ILP_solver.set_min();

    sp_ilp_settings_find_position_with_area_t settings(this);
    settings.set_constraints_modules_overlap_h();
    settings.set_constraints_modules_overlap_v();
    settings.set_constraints_modules_fixed();
    if (minimize_wirelength) {
        settings.set_constraints_net_direction();
        settings.set_constraints_net();
        settings.set_variables_connections();
    }
    settings.set_variables_modules();

    vector<double> coef(variable_n + 1, 0);
    if (minimize_wirelength) {
        settings.set_coef(coef);
    }
    ILP_solver.set_obj_coef(coef);
    const ILP_result_t result = ILP_solver.solve(false);
    if (!result.legal) {
        return false;
    }
    if (load_result) {
        modules_positions.resize(sequence_n);
        for (int i = 0; i < sequence_n; ++i) {
            modules_positions[i] =
                is_in_seq[i] == 0
                    ? vec2d_t(-1, -1)
                    // Rounded down to integers like the other LPs: this
                    // never creates overlaps (the shapes are integral).
                    : vec2d_t(std::floor(result.var_values[i + x_module_offset] + 1e-6),
                              std::floor(result.var_values[i + y_module_offset] + 1e-6));
        }
    }
    return true;
}

bool sequence_pair_t::find_position_allow_illegal_fill(
    bool minimize_wirelength, bool load_result) {
    build_constraint_graph();
    auto relaxable = [this](const edge_t& e) { return overlap_allowed(e); };
    reduce_transitive_edges_strict(constraint_graph_h, sequence_n, relaxable);
    reduce_transitive_edges_strict(constraint_graph_v, sequence_n, relaxable);
    set_shape_layout();
    // [.. find_position layout .. | x_overlap (per h edge) | y_overlap ..]
    const int c = static_cast<int>(connections.size());
    const int h_n = static_cast<int>(constraint_graph_h.size());
    const int v_n = static_cast<int>(constraint_graph_v.size());
    x_overlap = 1 + 2 * sequence_n + shape_n * soft_n + 4 * c;
    y_overlap = x_overlap + h_n;
    // NOTE: h_n more columns than needed are reserved (they stay fixed at 0);
    // kept to leave the LP identical to the original implementation.
    variable_n = 2 * sequence_n + shape_n * soft_n + 4 * c + 2 * h_n + v_n;
    if (!ILP_solver.init(variable_n)) {
        return false;
    }
    ILP_solver.set_min();

    sp_ilp_settings_find_position_t settings(this);
    settings.set_constraints_modules_allow_overlap_h();
    settings.set_constraints_modules_allow_overlap_v();
    settings.set_constraints_modules_overlap_content();
    settings.set_constraints_modules_fixed();
    if (minimize_wirelength) {
        settings.set_constraints_net_direction();
        settings.set_constraints_net();
    }
    settings.set_constraints_ratio_equal_1();
    settings.set_constraints_boundaries();
    settings.set_variables_modules();
    settings.set_variables_allow_overlap();
    if (minimize_wirelength) {
        settings.set_variables_connections();
    }
    settings.set_variables_shapes();

    vector<double> coef(variable_n + 1, 0);
    if (minimize_wirelength) {
        settings.set_coef(coef);  // overlaps are free
    }
    ILP_solver.set_obj_coef(coef);
    const ILP_result_t result = ILP_solver.solve(false);
    if (!result.legal) {
        lp_predicted_wirelength = -1;
        return false;
    }
    if (load_result) {
        load_LP_result(result);
    }
    return true;
}

bool sequence_pair_t::find_position_allow_illegal(bool minimize_wirelength,
                                                  bool load_result) {
    // 1st attempt: overlapping floorplan with the original areas.
    if (!find_position_allow_illegal_fill(minimize_wirelength, load_result)) {
        return find_position(true, true);
    }
    // 2nd attempt: enlarge the modules that lose area to overlaps.
    const vector<int> compensation = get_correct_compensation();
    vector<vector<vec2d_t>> compensated_shapes(soft_n);
    for (int i = 0; i < soft_n; ++i) {
        compensated_shapes[i] =
            find_w_h(modules_area[i] + compensation[i], shape_n);
    }
    std::swap(soft_area_to_w_h_m_5, compensated_shapes);
    const bool second_attempt =
        find_position_allow_illegal_fill(minimize_wirelength, load_result);
    std::swap(soft_area_to_w_h_m_5, compensated_shapes);
    if (!second_attempt) {
        return find_position(true, true);
    }
    // Fall back if the compensation made the overlaps worse.
    const vector<int> compensation_after = get_correct_area();
    for (int i = 0; i < sequence_n; ++i) {
        if (compensation_after[i] > compensation[i]) {
            return find_position(true, true);
        }
    }
    return true;
}

bool sequence_pair_t::find_position_allow_illegal_process() {
    const bool rectangular_ok = find_position(true, true);
    find_position_with_area(true, true);
    const double rectangular_wirelength = lp_predicted_wirelength;
    const bool overlap_ok = find_position_allow_illegal(true, true);
    const double overlap_wirelength = lp_predicted_wirelength;
    if (!rectangular_ok && !overlap_ok) {
        return false;
    }
    if (rectangular_ok != overlap_ok) {
        return true;
    }
    if (rectangular_wirelength < overlap_wirelength) {
        // The rectangular floorplan is better: restore it.
        find_position(true, true);
        find_position_with_area(true, true);
    }
    return true;
}

// ---------------------------------------------------------------------------
// Overlaps
// ---------------------------------------------------------------------------

vector<int> sequence_pair_t::get_overlapped_area() const {
    vector<rect_t> rects;
    rects.reserve(sequence_n);
    for (int i = 0; i < sequence_n; ++i) {
        rects.emplace_back(modules_positions[i], modules_wh[i]);
    }
    vector<int> result(sequence_n, 0);
    for (int i = 0; i < soft_n; ++i) {
        for (int j = i + 1; j < sequence_n; ++j) {
            if (rects[i].is_collision(rects[j]) &&
                modules_area[i] > modules_area[j]) {
                result[i] += rects[j].get_area();
            }
        }
    }
    return result;
}

vector<int> sequence_pair_t::get_correct_area() const {
    return get_overlapped_area();
}

vector<int> sequence_pair_t::get_correct_compensation() const {
    vector<int> result = get_overlapped_area();
    for (int i = 0; i < soft_n; ++i) {
        const double area = modules_wh[i].get_x() * modules_wh[i].get_y();
        if (area < 4 * result[i]) {
            result[i] += (4 * result[i] - area);
        }
    }
    return result;
}

bool sequence_pair_t::check_area_ratio() const {
    // Checks the areas and the 1:2 aspect ratios (not overlaps / 80% rule).
    const vector<int> area_compensation = get_correct_area();
    for (int i = 0; i < sequence_n; ++i) {
        if (seq_is_fix[i]) {
            continue;
        }
        const double area = modules_wh[i].get_x() * modules_wh[i].get_y();
        const double ratio = modules_wh[i].get_x() / modules_wh[i].get_y();
        if (ratio > 2 || ratio < 0.5) {
            return false;
        }
        if (area < modules_area[i] + area_compensation[i]) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// Rectilinear floorplan (interlocking)
// ---------------------------------------------------------------------------

void sequence_pair_t::to_rectilinear() {
    set_is_in_seq(1);
    fill_near();
    overlap_optimization();
    carve();
    get_wirelength();
    set_bounding_lines();
    get_wirelength_rectilinear();
    deal_bounding_line();
}

void sequence_pair_t::fill_near_axis(bool horizontal,
                                     const vector<int>& area_comp) {
    auto main = [horizontal](const vec2d_t& v) {
        return horizontal ? v.get_x() : v.get_y();
    };
    auto cross = [horizontal](const vec2d_t& v) {
        return horizontal ? v.get_y() : v.get_x();
    };
    auto& nears = horizontal ? near_x : near_y;
    auto& incident = horizontal ? near_x_of : near_y_of;
    for (int i = 0; i < sequence_n; ++i) {
        if (area_comp[i]) {
            continue;
        }
        for (int j = 0; j < sequence_n; ++j) {
            if (i == j || seq_is_fix[i] || seq_is_fix[j] || area_comp[j]) {
                continue;
            }
            if (!no_hand.empty() && (no_hand[i] || no_hand[j])) {
                continue;  // interlock dropped by legalize_rectilinear()
            }
            const vec2d_t &pi = modules_positions[i], &wi = modules_wh[i];
            const vec2d_t &pj = modules_positions[j], &wj = modules_wh[j];
            if (main(pi) + main(wi) != main(pj)) {
                continue;  // i does not touch j
            }
            const int lower_i = cross(pi), upper_i = cross(pi) + cross(wi);
            const int lower_j = cross(pj), upper_j = cross(pj) + cross(wj);
            if ((lower_i <= upper_j && lower_i >= lower_j) ||
                (upper_i <= upper_j && upper_i >= lower_j)) {
                // The depth h of the interlock keeps both modules legal.
                const int ih = std::min(main(wi) / 4.01,
                                        1.949 * cross(wi) - main(wi));
                const int jh = std::min(main(wj) / 4.01,
                                        1.949 * cross(wj) - main(wj));
                const int h = std::min(ih, jh);
                if (h < 1) {
                    // A module already close to the 1:2 aspect-ratio limit
                    // cannot take a hand (h <= 0 would make it illegal).
                    continue;
                }
                const int id = static_cast<int>(nears.size());
                nears.push_back({i, j, h});
                incident[i].push_back(id);
                incident[j].push_back(id);
            }
        }
    }
}

void sequence_pair_t::fill_near() {
    near_x.clear();
    near_y.clear();
    near_x_of.assign(sequence_n, vector<int>());
    near_y_of.assign(sequence_n, vector<int>());
    // Modules overlapping smaller modules are not interlocked.
    const vector<int> area_comp = get_correct_area();
    fill_near_axis(true, area_comp);
    fill_near_axis(false, area_comp);
}

void sequence_pair_t::overlap_optimization() {
    // MIP choosing the interlocks (at most one per module) which minimise the
    // wirelength. The module columns hold the centres of the modules.
    // [x | y | x_e_l | x_e_r | y_e_l | y_e_r | near_x | near_y]
    const int c = static_cast<int>(connections.size());
    x_module_offset = 1;
    y_module_offset = 1 + sequence_n;
    x_edge_offset_l = 1 + 2 * sequence_n;
    x_edge_offset_r = x_edge_offset_l + c;
    y_edge_offset_l = x_edge_offset_l + 2 * c;
    y_edge_offset_r = x_edge_offset_l + 3 * c;
    near_x_offset = 1 + 2 * sequence_n + 4 * c;
    near_y_offset = near_x_offset + static_cast<int>(near_x.size());
    variable_n = 2 * sequence_n + 4 * c + static_cast<int>(near_x.size()) +
                 static_cast<int>(near_y.size());
    ILP_solver.init(variable_n);

    sp_ilp_settings_find_position_t settings(this);
    settings.set_constraints_net_direction();
    settings.set_constraints_opt_nets();
    settings.set_constraints_opt_modules();
    settings.set_constraints_only_one_hand();
    settings.set_variables_connections();
    settings.set_variables_modules();
    settings.set_variables_hands();

    vector<double> coef(variable_n + 1, 0);
    settings.set_coef(coef);
    ILP_solver.set_obj_coef(coef);
    const ILP_result_t result = ILP_solver.solve(true);

    result_carving_x.clear();
    result_carving_y.clear();
    if (result.legal) {
        for (size_t i = 0; i < near_x.size(); ++i) {
            if (result.var_values[near_x_offset + i]) {
                result_carving_x.push_back(near_x[i]);
            }
        }
        for (size_t i = 0; i < near_y.size(); ++i) {
            if (result.var_values[near_y_offset + i]) {
                result_carving_y.push_back(near_y[i]);
            }
        }
        rectilinear_wirelength = result.z;
    } else {
        rectilinear_wirelength = actual_wirelength;
    }
    if (rectilinear_wirelength < 0) {  // GLPK issues
        rectilinear_wirelength = 1e300 + 1;
    }
}

bool sequence_pair_t::carve_pair(const near_t& near, bool horizontal) {
    const int from = near.from, to = near.to, h = near.h;
    const vec2d_t ll_from = modules_positions[from], from_wh = modules_wh[from];
    const vec2d_t ll_to = modules_positions[to], to_wh = modules_wh[to];

    // c1..c7 outline the interlocking "hand": the part [c1 c2 c3 c7] moves
    // from `from` to `to` and [c4 c5 c6 c7] from `to` to `from`.
    vec2d_t c1, c2, c3, c4, c5, c6, c7;
    vector<vec2d_t> from_add, from_minus, to_add, to_minus;
    if (horizontal) {
        const int top = std::min(ll_from.get_y() + from_wh.get_y(),
                                 ll_to.get_y() + to_wh.get_y());
        const int bottom = std::max(ll_from.get_y(), ll_to.get_y());
        const int mid = (top + bottom) / 2;
        if (top - bottom < 2) {
            return false;
        }
        const double x = ll_from.get_x() + from_wh.get_x();  // shared side
        c1 = vec2d_t(x, top), c2 = vec2d_t(x - h, top);
        c3 = vec2d_t(x - h, mid), c4 = vec2d_t(x + h, mid);
        c5 = vec2d_t(x + h, bottom), c6 = vec2d_t(x, bottom);
        c7 = vec2d_t(x, mid);
        from_add = {c4, c5, c6, c7}, from_minus = {c7, c3, c2, c1};
        to_add = {c7, c3, c2, c1}, to_minus = {c4, c5, c6, c7};
    } else {
        const int right = std::min(ll_from.get_x() + from_wh.get_x(),
                                   ll_to.get_x() + to_wh.get_x());
        const int left = std::max(ll_from.get_x(), ll_to.get_x());
        const int mid = (right + left) / 2;
        if (right - left < 2) {
            return false;
        }
        const double y = ll_from.get_y() + from_wh.get_y();  // shared side
        c1 = vec2d_t(left, y), c2 = vec2d_t(left, y + h);
        c3 = vec2d_t(mid, y + h), c4 = vec2d_t(mid, y - h);
        c5 = vec2d_t(right, y - h), c6 = vec2d_t(right, y);
        c7 = vec2d_t(mid, y);
        from_add = {c1, c2, c3, c7}, from_minus = {c7, c6, c5, c4};
        to_add = {c7, c6, c5, c4}, to_minus = {c1, c2, c3, c7};
    }

    auto reshape = [](int module, const vec2d_t& ll, const vec2d_t& wh,
                      const vector<vec2d_t>& add,
                      const vector<vec2d_t>& minus) {
        bounding_line_t bd(rect_t(ll, wh).get_bounding_rect());
        bd = bounding_line_t::merge(bd, bounding_line_t(add))
                 .difference_pos_line[0];
        bd = bounding_line_t::merge(bd, bounding_line_t(minus, false))
                 .difference_pos_line[0];
        return std::make_pair(bd.get_nodes(), "s" + std::to_string(module));
    };
    carved[from] = carved[to] = true;
    bounding_lines[from] = reshape(from, ll_from, from_wh, from_add, from_minus);
    bounding_lines[to] = reshape(to, ll_to, to_wh, to_add, to_minus);
    return true;
}

void sequence_pair_t::carve() {
    bounding_lines.clear();
    bounding_lines.resize(sequence_n);
    carved.assign(sequence_n, false);
    result_carving_x_enable.assign(result_carving_x.size(), false);
    result_carving_y_enable.assign(result_carving_y.size(), false);
    for (size_t i = 0; i < result_carving_x.size(); ++i) {
        result_carving_x_enable[i] = carve_pair(result_carving_x[i], true);
    }
    for (size_t i = 0; i < result_carving_y.size(); ++i) {
        result_carving_y_enable[i] = carve_pair(result_carving_y[i], false);
    }
}

void sequence_pair_t::set_bounding_lines() {
    if (static_cast<int>(carved.size()) < sequence_n) {
        carved.assign(sequence_n, 0);
        bounding_lines.resize(sequence_n);
    }
    for (int i = 0; i < sequence_n; ++i) {
        if (!carved[i]) {
            bounding_lines[i] = {get_4_points(modules_positions[i], modules_wh[i]),
                                 module_name(i)};
        }
    }
}

void sequence_pair_t::deal_bounding_line() {
    // Removes the remaining overlaps: a soft module gives way to the fixed
    // modules and to the smaller soft modules it overlaps.
    const int soft = static_cast<int>(chip_t::get_soft_modules().size());
    const int n = static_cast<int>(chip_t::get_total_module_n());

    std::vector<bounding_line_t> lines, neg_lines;
    for (int i = 0; i < n; ++i) {
        lines.emplace_back(bounding_lines[i].first);
        neg_lines.emplace_back(bounding_lines[i].first, false);
    }
    std::vector<std::vector<int>> give_way_to(soft);
    for (int i = 0; i < soft; ++i) {
        for (int j = i + 1; j < soft; ++j) {
            if (lines[i].collision(neg_lines[j])) {
                if (lines[i].get_area() < lines[j].get_area()) {
                    give_way_to[j].push_back(i);
                } else {
                    give_way_to[i].push_back(j);
                }
            }
        }
    }
    for (int i = 0; i < soft; ++i) {
        bounding_line_t bd0(bounding_lines[i].first);
        for (int j = soft; j < n; ++j) {
            const bounding_line_t bd1(bounding_lines[j].first, false);
            if (!bd0.collision(bd1)) {
                continue;
            }
            auto merged = bounding_line_t::merge(bd0, bd1);
            if (!merged.difference_pos_line.empty()) {
                bd0 = merged.difference_pos_line[0];
            }
        }
        for (const int j : give_way_to[i]) {
            const bounding_line_t bd1(bounding_lines[j].first, false);
            auto merged = bounding_line_t::merge(bd0, bd1);
            if (!merged.difference_pos_line.empty()) {
                bd0 = merged.difference_pos_line[0];
            }
        }
        bounding_lines[i] = {bd0.get_nodes(), bounding_lines[i].second};
    }
}

// ---------------------------------------------------------------------------
// Wirelength
// ---------------------------------------------------------------------------

namespace {

double hpwl(const vector<net_t>& nets, const vector<vec2d_t>& centers) {
    double sum = 0;
    for (const net_t& net : nets) {
        double x_min = 1e9, y_min = 1e9, x_max = -1, y_max = -1;
        for (const int v : net.nodes) {
            x_min = std::min(x_min, centers[v].get_x());
            x_max = std::max(x_max, centers[v].get_x());
            y_min = std::min(y_min, centers[v].get_y());
            y_max = std::max(y_max, centers[v].get_y());
        }
        sum += ((x_max - x_min) + (y_max - y_min)) * net.w;
    }
    return sum;
}

}  // namespace

double sequence_pair_t::get_wirelength() {
    vector<vec2d_t> centers(sequence_n);
    for (int i = 0; i < sequence_n; ++i) {
        centers[i] = {modules_positions[i].get_x() + modules_wh[i].get_half_x(),
                      modules_positions[i].get_y() + modules_wh[i].get_half_y()};
    }
    return actual_wirelength = hpwl(connections, centers);
}

double sequence_pair_t::get_wirelength_rectilinear() {
    vector<vec2d_t> centers(sequence_n);
    for (int i = 0; i < sequence_n; ++i) {
        centers[i] = {modules_positions[i].get_x() + modules_wh[i].get_half_x(),
                      modules_positions[i].get_y() + modules_wh[i].get_half_y()};
    }
    // An interlock of depth h moves both centres by h/2.
    for (size_t i = 0; i < result_carving_x.size(); ++i) {
        if (!result_carving_x_enable[i]) {
            continue;
        }
        const near_t& c = result_carving_x[i];
        centers[c.from] = {centers[c.from].get_x() + c.h / 2,
                           centers[c.from].get_y()};
        centers[c.to] = {centers[c.to].get_x() - c.h / 2, centers[c.to].get_y()};
    }
    for (size_t i = 0; i < result_carving_y.size(); ++i) {
        if (!result_carving_y_enable[i]) {
            continue;
        }
        const near_t& c = result_carving_y[i];
        centers[c.from] = {centers[c.from].get_x(),
                           centers[c.from].get_y() + c.h / 2};
        centers[c.to] = {centers[c.to].get_x(), centers[c.to].get_y() - c.h / 2};
    }
    return rectilinear_wirelength = hpwl(connections, centers);
}

// ---------------------------------------------------------------------------
// Output, visualisation & checks
// ---------------------------------------------------------------------------

void sequence_pair_t::sequence_pair_validation() {
    std::vector<std::pair<std::vector<vec2d_t>, std::string>> lines;
    for (int i = 0; i < sequence_n; ++i) {
        if (modules_positions[i].get_x() < 0 || modules_positions[i].get_y() < 0) {
            continue;
        }
        lines.push_back(
            {rect_t(modules_positions[i], modules_wh[i]).to_bounding_point(),
             module_name(i)});
    }
    visualizer_t::draw_bounding_line_connection(lines);
    cout << "Sequence Pair is printed" << endl;
}

void sequence_pair_t::sequence_pair_validation(int t) {
    vector<std::pair<rect_t, std::string>> rects;
    for (int i = 0; i < sequence_n; ++i) {
        if (modules_positions[i].get_x() < 0 || modules_positions[i].get_y() < 0) {
            continue;
        }
        rects.push_back({{modules_positions[i], modules_wh[i]}, module_name(i)});
    }
    visualizer_t::show_fp_rect_no_border(rects, "SQP, t = " + std::to_string(t));
}

void sequence_pair_t::plot_rectilinear() {
    visualizer_t::draw_bounding_line(bounding_lines);
    logger_t::info({"Rectangle wirelength: " +
                    logger_t::to_string_with_precision(actual_wirelength, 2)});
    logger_t::info({"Rectilinear wirelength: " +
                    logger_t::to_string_with_precision(rectilinear_wirelength, 2)});
    const double optimization =
        (actual_wirelength - rectilinear_wirelength) / actual_wirelength * 100;
    logger_t::info({logger_t::to_string_with_precision(optimization, 2) +
                    "% optimization"});
    logger_t::info({"Rectilinear Flooplan is printed!"});
    logger_t::action({"Press any key to continue"});
    fgetc(stdin);
}

namespace {

// Aspect ratio within [1:2, 2:1] and >= 80% of the bounding box used.
bool shape_legal(const bounding_line_t& bd, double* ratio = nullptr,
                 double* percent = nullptr) {
    const vec2d_t size = bd.get_bounding_rect().get_size();
    const double r = size.get_x() / size.get_y();
    const double p = bd.get_area() / bd.get_bounding_rect().get_area();
    if (ratio) *ratio = r;
    if (percent) *percent = p;
    return r <= 2 && r >= 0.5 && p >= 0.8;
}

}  // namespace

vector<int> sequence_pair_t::get_illegal_modules() const {
    vector<int> illegal;
    for (int i = 0; i < soft_n; ++i) {
        if (!shape_legal(bounding_line_t(bounding_lines[i].first))) {
            illegal.push_back(i);
        }
    }
    return illegal;
}

bool sequence_pair_t::overlap_allowed(const edge_t& e) const {
    if (!no_overlap.empty() &&
        no_overlap.count({std::min(e.from, e.to), std::max(e.from, e.to)})) {
        return false;
    }
    return sp_ilp_settings_find_position_t::overlap_allowed_by_area(e);
}

bool sequence_pair_t::legalize_rectilinear() {
    vector<int> illegal = get_illegal_modules();
    if (illegal.empty()) {
        return true;
    }
    no_hand.resize(sequence_n, 0);
    // Modules overlapping module i, the largest overlap first.
    auto overlaps_of = [this](int i) {
        const rect_t ri(modules_positions[i], modules_wh[i]);
        vector<pair<double, int>> result;
        for (int j = 0; j < sequence_n; ++j) {
            if (j == i) continue;
            const auto [hit, common] =
                ri.intersect(rect_t(modules_positions[j], modules_wh[j]));
            if (hit && common.get_area() > 0) {
                result.push_back({common.get_area(), j});
            }
        }
        std::sort(result.rbegin(), result.rend());
        return result;
    };
    std::set<pair<int, int>> infeasible;  // forbidding them failed
    sequence_pair_t best = *this;
    size_t best_illegal = illegal.size();
    // Every round adds a restriction or marks a pair infeasible: it ends.
    for (int round = 0; round < 4 * sequence_n && !illegal.empty(); ++round) {
        const sequence_pair_t before = *this;
        vector<pair<int, int>> forbidden;
        bool changed = false;
        for (const int i : illegal) {
            if (carved[i] && !no_hand[i]) {
                no_hand[i] = 1;  // 1st: drop the interlock of the module
                changed = true;
                continue;
            }
            for (const auto& [area, j] : overlaps_of(i)) {  // 2nd: one overlap
                const pair<int, int> key = {std::min(i, j), std::max(i, j)};
                if (!no_overlap.count(key) && !infeasible.count(key)) {
                    no_overlap.insert(key);
                    forbidden.push_back(key);
                    changed = true;
                    break;
                }
            }
        }
        if (!changed) {
            break;  // nothing left to revert
        }
        if (!forbidden.empty() && !find_position_allow_illegal_process()) {
            *this = before;  // undo the prohibitions of this round
            infeasible.insert(forbidden.begin(), forbidden.end());
            continue;
        }
        to_rectilinear();
        illegal = get_illegal_modules();
        if (illegal.size() < best_illegal) {
            best = *this;
            best_illegal = illegal.size();
        }
    }
    *this = best;
    return best_illegal == 0;
}

void sequence_pair_t::check_rectilinear_result(bool save) {
    const int soft = static_cast<int>(chip_t::get_soft_modules().size());
    std::fstream file;
    if (save) {
        file.open(io_handler_t::output_checker_file_name + "/check.txt",
                  std::fstream::out);
    }
    int total_vertexes = 0, total_edges = 0, total_vertexes270 = 0;
    bool legal = true;
    for (int i = 0; i < soft; ++i) {
        const bounding_line_t bd(bounding_lines[i].first);
        const double area = bd.get_area();
        double ratio, percent;
        const bool module_legal = shape_legal(bd, &ratio, &percent);
        const int vertexes = bd.get_vertex_count();
        const int edges = bd.get_edge_count();
        const int vertexes270 = bd.get_vertex270_count();
        total_vertexes += vertexes;
        total_edges += edges;
        total_vertexes270 += vertexes270;
        legal &= module_legal;
        if (save) {
            file << std::setw(3) << bounding_lines[i].second << ", ";
            file << (module_legal ? "TRUE" : "FALSE") << ", ";
            file << std::setprecision(16) << std::setw(8) << area << ", ";
            file << std::fixed << std::setw(5) << std::setprecision(3) << ratio
                 << ", ";
            file << std::fixed << std::setw(5) << std::setprecision(3)
                 << percent << ", ";
            file << std::defaultfloat << std::setprecision(16) << std::setw(2)
                 << vertexes << ", ";
            file << std::setw(2) << edges << ", ";
            file << std::setw(2) << vertexes270 << ", ";
            for (const auto& pos : bd.get_nodes()) {
                file << pos << " | ";
            }
            file << endl;
        }
    }
    if (legal) {
        logger_t::success({"Legal Resulting Floorplan!!"});
    } else {
        logger_t::error({"Illegal Resulting Floorplan!!"});
    }
    if (save) {
        file << rectilinear_wirelength << endl;
        file << total_vertexes << endl;
        file << total_edges << endl;
        file << total_vertexes270 << endl;
    }
}

void sequence_pair_t::write_inline() {
    std::fstream fout("../../output_SP.txt", std::ios::out | std::ios::trunc);
    fout << "v: ";
    print_list(fout, v_sequence);
    fout << " " << endl << "h: ";
    print_list(fout, h_sequence);
    fout << endl;
    fout << "wirelength : " << std::setprecision(16) << get_wirelength() << endl;
}

// ---------------------------------------------------------------------------
// Debug printing
// ---------------------------------------------------------------------------

void sequence_pair_t::print() const {
    for (int i = 0; i < sequence_n; ++i) {
        cout << "seq# " << i << ":" << (seq_is_fix[i] ? "fixed, " : "soft, ")
             << "shape : " << modules_wh[i] << endl;
    }
    print_v();
    print_h();
}

void sequence_pair_t::print_inline() const {
    cout << "Sequence Pair: " << endl;
    print_v();
    print_h();
}

void sequence_pair_t::print_v() const {
    cout << "v: ";
    print_list(cout, v_sequence);
    cout << " " << endl;
}

void sequence_pair_t::print_h() const {
    cout << "h: ";
    print_list(cout, h_sequence);
    cout << endl;
}

void sequence_pair_t::print_shapes_i() const {
    cout << "shapes of the SP : ";
    print_list(cout, modules_wh_i);
    cout << endl;
}

void sequence_pair_t::print_fix_sequence() const {
    cout << "v: ";
    print_list(cout, fix_v_sequence);
    cout << " h: ";
    print_list(cout, fix_h_sequence);
    cout << endl;
}

void sequence_pair_t::print_connections() const {
    for (const auto& net : connections) {
        cout << "net : " << endl;
        for (const int node : net.nodes) {
            cout << node << " ";
        }
        cout << endl;
    }
}

void sequence_pair_t::print_result() const {
    const vector<int> area_compensation = get_correct_area();
    for (int i = 0; i < sequence_n; ++i) {
        const double area = modules_wh[i].get_x() * modules_wh[i].get_y();
        cout << std::setprecision(16);
        cout << i << ", pos : " << modules_positions[i] << endl;
        cout << i << ", area: " << area << endl;
        cout << i << ", wh : " << modules_wh[i] << endl;
        cout << i << ", minimum area: " << modules_area[i] << endl;
        cout << i << ", area - minimum: " << area - modules_area[i] << endl;
        cout << i << ", area - compensation: " << area - area_compensation[i]
             << endl;
        cout << i << ", Ratio: " << std::setprecision(3)
             << (1 - static_cast<double>(area_compensation[i]) /
                         modules_area[i]) *
                    100
             << "%" << endl;
    }
}

void sequence_pair_t::print_wirelength() {
    cout << std::setprecision(16) << get_wirelength() << endl;
}
