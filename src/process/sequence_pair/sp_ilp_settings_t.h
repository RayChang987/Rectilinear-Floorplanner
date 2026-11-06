#pragma once
#include <vector>

#include "process/components/edge_t.h"

class sequence_pair_t;

// Builders of the LP / MIP models of a sequence pair.
//
// The column layout is defined by sequence_pair_t (the *_offset members);
// every set_constraints_* function appends rows to sp->ILP_solver.
// NOTE: the order of the rows influences which of several optimal solutions
// GLPK returns, so keep it stable.
class sp_ilp_settings_find_position_t {
   public:
    explicit sp_ilp_settings_find_position_t(sequence_pair_t* sp) : sp(sp) {}

    // Non-overlapping constraints of the constraint graphs:
    // x_from + w_from <= x_to (h) and y_from + h_from <= y_to (v).
    void set_constraints_modules_overlap_h();
    void set_constraints_modules_overlap_v();
    // Same, but pairs with very different areas may overlap; the amount of
    // overlap is bounded by set_constraints_modules_overlap_content().
    void set_constraints_modules_allow_overlap_h();
    void set_constraints_modules_allow_overlap_v();
    void set_constraints_modules_overlap_content();
    // Fixed modules stay at their positions.
    void set_constraints_modules_fixed();
    // Net bounding boxes: left <= right.
    void set_constraints_net_direction();
    // Net bounding boxes contain the centres of their modules.
    void set_constraints_net();
    // The shape weights of each soft module sum up to 1.
    void set_constraints_ratio_equal_1();
    // Soft modules stay inside the chip.
    void set_constraints_boundaries();
    void set_variables_modules();
    void set_variables_connections();
    void set_variables_shapes();
    void set_variables_allow_overlap();
    // Objective: weighted HPWL.
    void set_coef(std::vector<double>& coef);

    // Interlocking (rectilinear) optimisation
    void set_constraints_opt_nets();
    void set_constraints_opt_modules();
    void set_constraints_only_one_hand();
    void set_variables_hands();

    // Modules whose areas differ by at least this factor may overlap.
    static constexpr double area_overlap_constraint = 7;
    // Whether the constraint-graph edge `e` gets an overlap variable (area
    // rule only; see sequence_pair_t::overlap_allowed()).
    static bool overlap_allowed_by_area(const edge_t& e);

   private:
    void set_constraints_modules_overlap(bool horizontal);
    void set_constraints_modules_allow_overlap(bool horizontal);
    void set_constraints_opt_modules_axis(int i, bool horizontal);

    sequence_pair_t* sp;
};

// Model with given module shapes (sequence_pair_t::modules_wh).
class sp_ilp_settings_find_position_with_area_t {
   public:
    explicit sp_ilp_settings_find_position_with_area_t(sequence_pair_t* sp)
        : sp(sp) {}

    void set_constraints_modules_overlap_h();
    void set_constraints_modules_overlap_v();
    void set_constraints_modules_fixed();
    void set_constraints_net_direction();
    void set_constraints_net();
    void set_variables_modules();
    void set_variables_connections();
    void set_coef(std::vector<double>& coef);

   private:
    sequence_pair_t* sp;
};
