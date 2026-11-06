#include "sp_ilp_settings_t.h"

#include <cmath>

#include "sequence_pair_t.h"

namespace {

using sp_t = sequence_pair_t;

double length(const vec2d_t& v, bool horizontal) {
    return horizontal ? v.get_x() : v.get_y();
}

double half_length(const vec2d_t& v, bool horizontal) {
    return horizontal ? v.get_half_x() : v.get_half_y();
}

double chip_length(bool horizontal) {
    return horizontal ? chip_t::get_width() : chip_t::get_height();
}

}  // namespace

// ---------------------------------------------------------------------------
// sp_ilp_settings_find_position_t
// ---------------------------------------------------------------------------

bool sp_ilp_settings_find_position_t::overlap_allowed_by_area(const edge_t& e) {
    const double from_area = sp_t::modules_area[e.from];
    const double to_area = sp_t::modules_area[e.to];
    const double ratio = from_area / to_area;
    if (sp_t::seq_is_fix[e.from] && from_area >= to_area) {
        return false;
    }
    if (sp_t::seq_is_fix[e.to] && to_area >= from_area) {
        return false;
    }
    return ratio >= area_overlap_constraint ||
           ratio <= 1 / area_overlap_constraint;
}

void sp_ilp_settings_find_position_t::set_constraints_modules_overlap(
    bool horizontal) {
    const auto& graph =
        horizontal ? sp->constraint_graph_h : sp->constraint_graph_v;
    const int off = horizontal ? sp->x_module_offset : sp->y_module_offset;
    const int* st = sp->shape_types;
    for (const edge_t& e : graph) {
        const int from = e.from, to = e.to;
        if (sp_t::seq_is_fix[from]) {
            sp->ILP_solver.add_constraint_upb({from + off, to + off}, {1, -1},
                                              -e.w);
        } else {
            const auto& s = sp->soft_area_to_w_h_m_5[from];
            sp->ILP_solver.add_constraint_upb(
                {from + off, to + off, st[0] + from, st[1] + from,
                 st[2] + from, st[3] + from, st[4] + from},
                {1, -1, length(s[0], horizontal), length(s[1], horizontal),
                 length(s[2], horizontal), length(s[3], horizontal),
                 length(s[4], horizontal)},
                -1e-5);
        }
    }
}

void sp_ilp_settings_find_position_t::set_constraints_modules_overlap_h() {
    set_constraints_modules_overlap(true);
}

void sp_ilp_settings_find_position_t::set_constraints_modules_overlap_v() {
    set_constraints_modules_overlap(false);
}

void sp_ilp_settings_find_position_t::set_constraints_modules_allow_overlap(
    bool horizontal) {
    const auto& graph =
        horizontal ? sp->constraint_graph_h : sp->constraint_graph_v;
    const int off = horizontal ? sp->x_module_offset : sp->y_module_offset;
    const int ov_off = horizontal ? sp->x_overlap : sp->y_overlap;
    const int* st = sp->shape_types;
    for (int i = 0; i < static_cast<int>(graph.size()); ++i) {
        const int from = graph[i].from, to = graph[i].to;
        const double overlap_var = sp->overlap_allowed(graph[i]) ? -1.0 : 0.0;
        if (sp_t::seq_is_fix[from]) {
            sp->ILP_solver.add_constraint_upb(
                {from + off, to + off, i + ov_off}, {1, -1, overlap_var},
                -graph[i].w);
            continue;  // the pair constraints below need two soft modules
        }
        const auto& s = sp->soft_area_to_w_h_m_5[from];
        sp->ILP_solver.add_constraint_upb(
            {from + off, to + off, i + ov_off, st[0] + from, st[1] + from,
             st[2] + from, st[3] + from, st[4] + from},
            {1, -1, overlap_var, length(s[0], horizontal),
             length(s[1], horizontal), length(s[2], horizontal),
             length(s[3], horizontal), length(s[4], horizontal)},
            -1e-5);
        if (sp_t::seq_is_fix[to]) {
            continue;
        }
        // Even when overlapping, `to` must start and end after `from`.
        const auto& t = sp->soft_area_to_w_h_m_5[to];
        std::vector<int> cols = {from + off, to + off};
        std::vector<double> values = {1, -1};
        for (int k = 0; k < sp_t::shape_n; ++k) {
            cols.push_back(st[k] + from);
            values.push_back(length(s[k], horizontal));
        }
        for (int k = 0; k < sp_t::shape_n; ++k) {
            cols.push_back(st[k] + to);
            values.push_back(-length(t[k], horizontal));
        }
        sp->ILP_solver.add_constraint_upb(cols, values, -1);
        sp->ILP_solver.add_constraint_upb({from + off, to + off}, {1, -1}, -1);
    }
}

void sp_ilp_settings_find_position_t::
    set_constraints_modules_allow_overlap_h() {
    set_constraints_modules_allow_overlap(true);
}

void sp_ilp_settings_find_position_t::
    set_constraints_modules_allow_overlap_v() {
    set_constraints_modules_allow_overlap(false);
}

void sp_ilp_settings_find_position_t::
    set_constraints_modules_overlap_content() {
    // The overlap of an edge is charged to the larger of its two modules.
    auto charged_overlaps = [](const std::vector<edge_t>& graph, int module,
                               int ov_off) {
        std::vector<int> cols;
        for (int j = 0; j < static_cast<int>(graph.size()); ++j) {
            const int from_area =
                static_cast<int>(sp_t::modules_area[graph[j].from]);
            const int to_area = static_cast<int>(sp_t::modules_area[graph[j].to]);
            if ((graph[j].from == module && from_area > to_area) ||
                (graph[j].to == module && to_area > from_area)) {
                cols.push_back(ov_off + j);
            }
        }
        return cols;
    };
    for (int i = 0; i < sp_t::sequence_n; ++i) {
        const double max_overlap = std::sqrt(sp_t::modules_area[i]) * 0.5;
        const auto ph = charged_overlaps(sp->constraint_graph_h, i, sp->x_overlap);
        sp->ILP_solver.add_constraint_upb(
            ph, std::vector<double>(ph.size(), 1), max_overlap);
        const auto pv = charged_overlaps(sp->constraint_graph_v, i, sp->y_overlap);
        sp->ILP_solver.add_constraint_upb(
            pv, std::vector<double>(pv.size(), 1), max_overlap);
    }
}

void sp_ilp_settings_find_position_t::set_constraints_modules_fixed() {
    for (int i = 0; i < sp_t::sequence_n; ++i) {
        if (!sp_t::seq_is_fix[i]) {
            continue;
        }
        const vec2d_t ll = sp_t::seq_fixed_map[i]->get_left_lower();
        sp->ILP_solver.add_constraint_fx({i + sp->x_module_offset}, {1},
                                         static_cast<int>(ll.get_x()));
        sp->ILP_solver.add_constraint_fx({i + sp->y_module_offset}, {1},
                                         static_cast<int>(ll.get_y()));
    }
}

void sp_ilp_settings_find_position_t::set_constraints_net_direction() {
    for (int i = 0; i < static_cast<int>(sp_t::connections.size()); ++i) {
        sp->ILP_solver.add_constraint_upb(
            {sp->x_edge_offset_l + i, sp->x_edge_offset_r + i}, {1, -1}, 0.0);
        sp->ILP_solver.add_constraint_upb(
            {sp->y_edge_offset_l + i, sp->y_edge_offset_r + i}, {1, -1}, 0.0);
    }
}

void sp_ilp_settings_find_position_t::set_constraints_net() {
    const int* st = sp->shape_types;
    const int xm = sp->x_module_offset, ym = sp->y_module_offset;
    for (int i = 0; i < static_cast<int>(sp_t::connections.size()); ++i) {
        const int xl = sp->x_edge_offset_l + i, xr = sp->x_edge_offset_r + i;
        const int yl = sp->y_edge_offset_l + i, yr = sp->y_edge_offset_r + i;
        for (const int b : sp_t::connections[i].nodes) {
            if (sp_t::seq_is_fix[b]) {
                const double bx = sp->modules_wh[b].get_half_x();
                const double by = sp->modules_wh[b].get_half_y();
                sp->ILP_solver.add_constraint_upb({xl, b + xm}, {1, -1}, bx);
                sp->ILP_solver.add_constraint_upb({yl, b + ym}, {1, -1}, by);
                sp->ILP_solver.add_constraint_upb({b + xm, xr}, {1, -1}, -bx);
                sp->ILP_solver.add_constraint_upb({b + ym, yr}, {1, -1}, -by);
                continue;
            }
            // Centre of a soft module: x + sum(w_k / 2 * shape_k)
            const auto& s = sp->soft_area_to_w_h_m_5[b];
            double bw[sp_t::shape_n], bh[sp_t::shape_n];
            for (int k = 0; k < sp_t::shape_n; ++k) {
                bw[k] = half_length(s[k], true);
                bh[k] = half_length(s[k], false);
            }
            const int s0 = b + st[0], s1 = b + st[1], s2 = b + st[2],
                      s3 = b + st[3], s4 = b + st[4];
            sp->ILP_solver.add_constraint_upb(
                {xl, b + xm, s0, s1, s2, s3, s4},
                {1, -1, -bw[0], -bw[1], -bw[2], -bw[3], -bw[4]}, 0);
            sp->ILP_solver.add_constraint_upb(
                {yl, b + ym, s0, s1, s2, s3, s4},
                {1, -1, -bh[0], -bh[1], -bh[2], -bh[3], -bh[4]}, 0);
            sp->ILP_solver.add_constraint_upb(
                {b + xm, xr, s0, s1, s2, s3, s4},
                {1, -1, bw[0], bw[1], bw[2], bw[3], bw[4]}, 0);
            sp->ILP_solver.add_constraint_upb(
                {b + ym, yr, s0, s1, s2, s3, s4},
                {1, -1, bh[0], bh[1], bh[2], bh[3], bh[4]}, 0);
        }
    }
}

void sp_ilp_settings_find_position_t::set_constraints_ratio_equal_1() {
    const int* st = sp->shape_types;
    for (int i = 0; i < sp_t::soft_n; ++i) {
        sp->ILP_solver.add_constraint_fx(
            {st[0] + i, st[1] + i, st[2] + i, st[3] + i, st[4] + i},
            {1, 1, 1, 1, 1}, 1.001);
    }
}

void sp_ilp_settings_find_position_t::set_constraints_boundaries() {
    const int* st = sp->shape_types;
    for (int i = 0; i < sp_t::soft_n; ++i) {
        const auto& s = sp->soft_area_to_w_h_m_5[i];
        for (const bool horizontal : {true, false}) {
            const int off =
                horizontal ? sp->x_module_offset : sp->y_module_offset;
            sp->ILP_solver.add_constraint_upb(
                {off + i, st[0] + i, st[1] + i, st[2] + i, st[3] + i,
                 st[4] + i},
                {1, length(s[0], horizontal), length(s[1], horizontal),
                 length(s[2], horizontal), length(s[3], horizontal),
                 length(s[4], horizontal)},
                chip_length(horizontal));
        }
    }
}

void sp_ilp_settings_find_position_t::set_variables_modules() {
    for (int i = 0; i < sp_t::sequence_n; ++i) {
        sp->ILP_solver.set_variable_double_range(i + sp->x_module_offset, 0.0,
                                                 chip_t::get_width());
        sp->ILP_solver.set_variable_double_range(i + sp->y_module_offset, 0.0,
                                                 chip_t::get_height());
    }
}

void sp_ilp_settings_find_position_t::set_variables_allow_overlap() {
    for (int i = 0; i < static_cast<int>(sp->constraint_graph_h.size()); ++i) {
        sp->ILP_solver.set_variable_double_range(sp->x_overlap + i, 0.0,
                                                 chip_t::get_width());
    }
    for (int i = 0; i < static_cast<int>(sp->constraint_graph_v.size()); ++i) {
        sp->ILP_solver.set_variable_double_range(sp->y_overlap + i, 0.0,
                                                 chip_t::get_height());
    }
}

void sp_ilp_settings_find_position_t::set_variables_connections() {
    for (int i = 0; i < static_cast<int>(sp_t::connections.size()); ++i) {
        sp->ILP_solver.set_variable_double_range(sp->x_edge_offset_l + i, 0.0,
                                                 chip_t::get_width());
        sp->ILP_solver.set_variable_double_range(sp->x_edge_offset_r + i, 0.0,
                                                 chip_t::get_width());
        sp->ILP_solver.set_variable_double_range(sp->y_edge_offset_l + i, 0.0,
                                                 chip_t::get_height());
        sp->ILP_solver.set_variable_double_range(sp->y_edge_offset_r + i, 0.0,
                                                 chip_t::get_height());
    }
}

void sp_ilp_settings_find_position_t::set_variables_shapes() {
    for (int i = 0; i < sp_t::soft_n; ++i) {
        for (int j = 0; j < sp_t::shape_n; ++j) {
            sp->ILP_solver.set_variable_double_range(sp->shape_types[j] + i, 0,
                                                     1);
        }
    }
}

void sp_ilp_settings_find_position_t::set_coef(std::vector<double>& coef) {
    for (int i = 0; i < static_cast<int>(sp_t::connections.size()); ++i) {
        const double w = sp_t::connections[i].w;
        coef[i + sp->x_edge_offset_r] = w;
        coef[i + sp->x_edge_offset_l] = -w;
        coef[i + sp->y_edge_offset_r] = w;
        coef[i + sp->y_edge_offset_l] = -w;
    }
}

void sp_ilp_settings_find_position_t::set_constraints_opt_nets() {
    // The net bounding box contains the centres of all its modules.
    for (int i = 0; i < static_cast<int>(sp_t::connections.size()); ++i) {
        const auto& nodes = sp_t::connections[i].nodes;
        for (const int u : nodes) {
            sp->ILP_solver.add_constraint_upb(
                {sp->x_module_offset + u, sp->x_edge_offset_r + i}, {1, -1}, 0);
        }
        for (const int u : nodes) {
            sp->ILP_solver.add_constraint_upb(
                {sp->x_edge_offset_l + i, sp->x_module_offset + u}, {1, -1}, 0);
        }
        for (const int u : nodes) {
            sp->ILP_solver.add_constraint_upb(
                {sp->y_module_offset + u, sp->y_edge_offset_r + i}, {1, -1}, 0);
        }
        for (const int u : nodes) {
            sp->ILP_solver.add_constraint_upb(
                {sp->y_edge_offset_l + i, sp->y_module_offset + u}, {1, -1}, 0);
        }
    }
}

void sp_ilp_settings_find_position_t::set_constraints_opt_modules_axis(
    int i, bool horizontal) {
    // The centre of module i moves by +-h/2 for every interlock it takes part
    // in (the column of the module holds its centre here).
    const auto& nears = horizontal ? sp->near_x : sp->near_y;
    const auto& incident = horizontal ? sp->near_x_of[i] : sp->near_y_of[i];
    const int near_off = horizontal ? sp->near_x_offset : sp->near_y_offset;
    const double center = length(sp->modules_positions[i], horizontal) +
                          half_length(sp->modules_wh[i], horizontal);
    std::vector<int> cols;
    std::vector<double> values;
    for (const int id : incident) {
        const int h = nears[id].h;
        values.push_back(nears[id].from == i ? h / 2 : -h / 2);
        cols.push_back(id + near_off);
    }
    values.push_back(-1);
    cols.push_back(
        (horizontal ? sp->x_module_offset : sp->y_module_offset) + i);
    sp->ILP_solver.add_constraint_fx(cols, values, -center);
}

void sp_ilp_settings_find_position_t::set_constraints_opt_modules() {
    for (int i = 0; i < sp_t::sequence_n; ++i) {
        set_constraints_opt_modules_axis(i, true);
        set_constraints_opt_modules_axis(i, false);
    }
}

void sp_ilp_settings_find_position_t::set_constraints_only_one_hand() {
    // Every soft module takes part in at most one interlock.
    for (int i = 0; i < sp_t::sequence_n; ++i) {
        if (sp_t::seq_is_fix[i]) {
            continue;
        }
        std::vector<int> cols;
        for (const int id : sp->near_x_of[i]) {
            cols.push_back(id + sp->near_x_offset);
        }
        for (const int id : sp->near_y_of[i]) {
            cols.push_back(id + sp->near_y_offset);
        }
        if (!cols.empty()) {
            sp->ILP_solver.add_constraint_upb(
                cols, std::vector<double>(cols.size(), 1), 1);
        }
    }
}

void sp_ilp_settings_find_position_t::set_variables_hands() {
    for (int i = 0; i < static_cast<int>(sp->near_x.size()); ++i) {
        sp->ILP_solver.set_variable_double_range_int(sp->near_x_offset + i, 0,
                                                     1);
    }
    for (int i = 0; i < static_cast<int>(sp->near_y.size()); ++i) {
        sp->ILP_solver.set_variable_double_range_int(sp->near_y_offset + i, 0,
                                                     1);
    }
}

// ---------------------------------------------------------------------------
// sp_ilp_settings_find_position_with_area_t
// ---------------------------------------------------------------------------

void sp_ilp_settings_find_position_with_area_t::
    set_constraints_modules_overlap_h() {
    for (const edge_t& e : sp->constraint_graph_h) {
        sp->ILP_solver.add_constraint_upb(
            {e.from + sp->x_module_offset, e.to + sp->x_module_offset},
            {1, -1}, -e.w);
    }
}

void sp_ilp_settings_find_position_with_area_t::
    set_constraints_modules_overlap_v() {
    for (const edge_t& e : sp->constraint_graph_v) {
        sp->ILP_solver.add_constraint_upb(
            {e.from + sp->y_module_offset, e.to + sp->y_module_offset},
            {1, -1}, -e.w);
    }
}

void sp_ilp_settings_find_position_with_area_t::
    set_constraints_modules_fixed() {
    sp_ilp_settings_find_position_t(sp).set_constraints_modules_fixed();
}

void sp_ilp_settings_find_position_with_area_t::
    set_constraints_net_direction() {
    sp_ilp_settings_find_position_t(sp).set_constraints_net_direction();
}

void sp_ilp_settings_find_position_with_area_t::set_constraints_net() {
    const int xm = sp->x_module_offset, ym = sp->y_module_offset;
    for (int i = 0; i < static_cast<int>(sp_t::connections.size()); ++i) {
        const int xl = sp->x_edge_offset_l + i, xr = sp->x_edge_offset_r + i;
        const int yl = sp->y_edge_offset_l + i, yr = sp->y_edge_offset_r + i;
        for (const int b : sp_t::connections[i].nodes) {
            const double bx = sp->modules_wh[b].get_half_x();
            const double by = sp->modules_wh[b].get_half_y();
            sp->ILP_solver.add_constraint_upb({xl, b + xm}, {1, -1}, bx);
            sp->ILP_solver.add_constraint_upb({yl, b + ym}, {1, -1}, by);
            sp->ILP_solver.add_constraint_upb({b + xm, xr}, {1, -1}, -bx);
            sp->ILP_solver.add_constraint_upb({b + ym, yr}, {1, -1}, -by);
        }
    }
}

void sp_ilp_settings_find_position_with_area_t::set_variables_modules() {
    for (int i = 0; i < sp_t::sequence_n; ++i) {
        sp->ILP_solver.set_variable_double_range(
            sp->x_module_offset + i, 0.0,
            chip_t::get_width() - sp->modules_wh[i].get_x());
    }
    for (int i = 0; i < sp_t::sequence_n; ++i) {
        sp->ILP_solver.set_variable_double_range(
            sp->y_module_offset + i, 0.0,
            chip_t::get_height() - sp->modules_wh[i].get_y());
    }
}

void sp_ilp_settings_find_position_with_area_t::set_variables_connections() {
    sp_ilp_settings_find_position_t(sp).set_variables_connections();
}

void sp_ilp_settings_find_position_with_area_t::set_coef(
    std::vector<double>& coef) {
    sp_ilp_settings_find_position_t(sp).set_coef(coef);
}
