//
// Created by RayChang on 2023/5/18.
//

#ifndef ICCAD2023PD_ILP_SOLVER_T_H
#define ICCAD2023PD_ILP_SOLVER_T_H

#include <cstddef>
#include <initializer_list>
#include <vector>

class ILP_result_t {
   public:
    bool legal = false;
    double z = 0;
    // Column values, 1-indexed (index 0 is unused). Empty if !legal.
    std::vector<double> var_values;
};

// A small model builder around GLPK.
//
// Rows, columns, the objective and the sparse matrix are buffered in vectors;
// the GLPK problem object only exists inside solve(). The class therefore has
// no manual memory management and is safe to copy.
//
// Rows are numbered from 1 in the order they are added. Columns are numbered
// 1..variable_n; a column that is never configured is fixed at 0 (GLPK's own
// default for a new column).
class ILP_solver_t {
   public:
    // Starts a new model. Returns false (invalid model) if variable_n <= 0.
    bool init(int variable_n);
    bool get_is_invalid() const { return invalid_input; }

    void set_min() { maximize = false; }
    void set_max() { maximize = true; }

    // sum(values[k] * x[cols[k]]) <= upb
    void add_constraint_upb(std::initializer_list<int> cols,
                            std::initializer_list<double> values, double upb);
    void add_constraint_upb(const std::vector<int>& cols,
                            const std::vector<double>& values, double upb);
    // sum(values[k] * x[cols[k]]) == fix
    void add_constraint_fx(std::initializer_list<int> cols,
                           std::initializer_list<double> values, double fix);
    void add_constraint_fx(const std::vector<int>& cols,
                           const std::vector<double>& values, double fix);

    // lb <= x[var_i] <= ub (a fixed column if lb == ub)
    void set_variable_double_range(int var_i, double lb, double ub);
    // Same as above for an integer column.
    void set_variable_double_range_int(int var_i, double lb, double ub);

    // coef[i] is the objective coefficient of column i (coef[0] is ignored).
    void set_obj_coef(const std::vector<double>& coef);

    // Solves the model as a MIP (int_needed) or an LP and clears the builder.
    ILP_result_t solve(bool int_needed);

   private:
    void add_row(const int* cols, const double* values, std::size_t n,
                 int type, double lb, double ub);
    void reset();

    bool invalid_input = false;
    bool maximize = false;
    int var_n = 0;

    std::vector<int> row_type;
    std::vector<double> row_lb, row_ub;
    std::vector<int> col_type, col_kind;
    std::vector<double> col_lb, col_ub, obj;
    // Matrix triplets, 1-indexed as required by glp_load_matrix.
    std::vector<int> mat_i{0}, mat_j{0};
    std::vector<double> mat_val{0.0};
};

#endif  // ICCAD2023PD_ILP_SOLVER_T_H
