//
// Created by RayChang on 2023/5/6.
//

#ifndef ICCAD2023PD_SEQUENCE_PAIR_T_H
#define ICCAD2023PD_SEQUENCE_PAIR_T_H
#include <cstdint>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "process/components/edge_t.h"
#include "process/components/net_t.h"
#include "process/functional/ILP_solver_t.h"
#include "static_data/chip_t.h"
#include "static_data/fixed_module_t.h"
#include "static_data/soft_module_t.h"
#include "utilities/vec2d_t.h"

using std::pair;
using std::vector;

class sequence_pair_t {
    friend class sp_ilp_settings_find_position_t;
    friend class sp_ilp_settings_find_position_with_area_t;

   public:
    // Number of shape samples per soft module used by the LP formulation.
    static constexpr int shape_n = 5;

    sequence_pair_t();

    // ---------------------------------------------------------------------
    // Static (per chip) data
    // ---------------------------------------------------------------------
    static void init();
    static void build_graph();
    // Legal (w, h) samples of a module with the given area.
    static vector<vec2d_t> find_w_h(uint32_t area, int resolution);

    static int sequence_n;            // Number of modules
    static int fixed_n;               // Number of fixed modules
    static int soft_n;                // Number of soft modules
    static int connection_graph_deg;  // Sum of the degrees of all nets
    static int fix_start_idx;  // The sequence number of the first fix module
    static vector<net_t> connections;  // All nets
    static vector<pair<int, int>> deg_w;
    // Whether a sequence number is a fixed module ([0,...,0,1,...,1]).
    static vector<bool> seq_is_fix;
    static vector<soft_module_t*> seq_soft_map;    // seq# -> soft module
    static vector<fixed_module_t*> seq_fixed_map;  // seq# -> fixed module
    static vector<double> modules_area;            // indexed by seq#

    // ---------------------------------------------------------------------
    // Sequence pair
    // ---------------------------------------------------------------------
    vector<int> h_sequence, v_sequence, fix_v_sequence, fix_h_sequence;

    // shape_n shape samples of every soft module
    vector<vector<vec2d_t>> soft_area_to_w_h_m_5;

    // Resulting floorplan
    vector<vec2d_t> modules_wh;          // shapes
    vector<int> modules_wh_i;            // index of the chosen sample or -1
    vector<vec2d_t> modules_positions;   // lower-left corners
    double actual_wirelength = -1;       // HPWL without interlocking
    double rectilinear_wirelength = -1;  // HPWL after interlocking
    double lp_predicted_wirelength = -1;  // objective value of the LP

    // Sequence pair initialisation
    vector<int> add_soft_order;  // insertion order of the soft modules
    vector<int> is_in_seq;       // whether a seq# is inserted

    // Rectilinear floorplan
    std::vector<std::pair<std::vector<vec2d_t>, std::string>> bounding_lines;
    vector<int> carved;

    // Functions with the prefix 'find_position' compute the floorplan of the
    // sequence pair and return whether it is legal.
    //
    // Shapes are chosen by the LP (convex combination of the samples).
    bool find_position(bool minimize_wirelength, bool load_result);
    // Shapes are taken from modules_wh.
    bool find_position_with_area(bool minimize_wirelength, bool load_result);
    // The better (lower wirelength) of the rectangular and the overlapping
    // (to be interlocked) floorplan.
    bool find_position_allow_illegal_process();
    // Overlapping floorplan with area compensation; falls back to the
    // rectangular floorplan if this fails.
    bool find_position_allow_illegal(bool minimize_wirelength,
                                     bool load_result);
    // The LP that allows bounded overlaps between modules.
    bool find_position_allow_illegal_fill(bool minimize_wirelength,
                                          bool load_result);

    void to_rectilinear();    // Interlock the overlapping modules
    void plot_rectilinear();  // Requires 'bounding_lines'

    // Soft modules whose rectilinear shape (bounding_lines) violates the
    // aspect-ratio (1:2 .. 2:1) or the 80% utilisation rule.
    vector<int> get_illegal_modules() const;
    // Repairs only the illegal modules of a rectilinear floorplan, reverting
    // as little as possible: first the interlock of an illegal module is
    // dropped; if it is still illegal, the overlap that cuts away most of its
    // area is forbidden (one pair at a time) and the floorplan is re-solved.
    // Pairs whose prohibition makes the floorplan infeasible are skipped.
    // All other interlocks and overlaps are kept. Returns true if legal.
    bool legalize_rectilinear();

    // Whether the constraint-graph edge `e` may be relaxed by an overlap.
    bool overlap_allowed(const edge_t& e) const;

    // Checks the rectilinear floorplan and optionally writes check.txt
    void check_rectilinear_result(bool save);

    // get & set
    void swap_v(int, int);
    void swap_h(int, int);
    void set_v(std::vector<int>);
    void set_h(std::vector<int>);
    void set_vi(int, int);
    void set_hi(int, int);
    void set_module_size(int, int);
    void set_is_in_seq(int);
    int get_vi(int) const;
    int get_hi(int) const;
    std::vector<int> get_v() const;
    std::vector<int> get_h() const;
    static vector<vec2d_t> get_4_points(const vec2d_t& ll, const vec2d_t& wh);
    vector<int> get_correct_area() const;
    vector<int> get_correct_compensation() const;
    double get_wirelength();
    double get_wirelength_rectilinear();
    void fill_near();
    void overlap_optimization();
    void carve();
    void deal_bounding_line();
    void change_size(int);
    void print_inline() const;
    void write_inline();
    void sequence_pair_validation();
    void sequence_pair_validation(int);
    void print_result() const;
    void set_bounding_lines();
    bool check_area_ratio() const;

    // Initialization
    void set_only_fix();
    bool is_completed();  // All modules inserted and the SP is solvable
    void init_modules_size();
    void set_fix_sequence();
    void set_add_order();

    // Constraint graphs
    void build_constraint_graph();
    void simplify_constraint_graph();

    // Debug
    void print() const;
    void print_v() const;
    void print_h() const;
    void print_shapes_i() const;
    void print_fix_sequence() const;
    void print_connections() const;
    void print_wirelength();

   private:
    // Pair of abutting modules which may be interlocked.
    struct near_t {
        int from, to, h;
    };

    // Shared by find_position() and find_position_allow_illegal_fill().
    void set_shape_layout();
    vector<vec2d_t> get_LP_res_pos(const ILP_result_t& result) const;
    vector<vec2d_t> get_LP_res_wh(const ILP_result_t& result) const;
    void load_LP_result(const ILP_result_t& result);
    // Sum of the areas of the smaller modules overlapping each module.
    vector<int> get_overlapped_area() const;
    // Interlocking candidates along one axis (see fill_near()).
    void fill_near_axis(bool horizontal, const vector<int>& area_comp);
    // Interlocks one pair; returns false if their common side is too short.
    bool carve_pair(const near_t& near, bool horizontal);

    // LP layout (1-indexed GLPK columns)
    int variable_n = 0;
    int x_module_offset = 0, y_module_offset = 0;
    int x_edge_offset_l = 0, x_edge_offset_r = 0;
    int y_edge_offset_l = 0, y_edge_offset_r = 0;
    int x_overlap = 0, y_overlap = 0;
    int near_x_offset = 0, near_y_offset = 0;
    int shape_types[shape_n] = {};

    // near_x: the right side of `from` touches the left side of `to`
    // near_y: the top side of `from` touches the bottom side of `to`
    vector<near_t> near_x, near_y;
    // ids of the near_* pairs incident to each module, in insertion order
    vector<vector<int>> near_x_of, near_y_of;

    // Restrictions set by legalize_rectilinear(): modules without interlock
    // (indexed by seq#, empty: none) and pairs (a < b) that may not overlap.
    vector<char> no_hand;
    std::set<std::pair<int, int>> no_overlap;

    ILP_solver_t ILP_solver;
    vector<near_t> result_carving_x, result_carving_y;
    vector<bool> result_carving_x_enable, result_carving_y_enable;
    vector<edge_t> constraint_graph_h, constraint_graph_v;
};

#endif  // ICCAD2023PD_SEQUENCE_PAIR_T_H
