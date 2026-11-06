//
// Created by RayChang on 2023/5/18.
//

#include "ILP_solver_t.h"

#include <glpk.h>

#include <cassert>

bool ILP_solver_t::init(int variable_n) {
    reset();
    if (variable_n <= 0) {
        invalid_input = true;
        return false;
    }
    var_n = variable_n;
    col_type.assign(var_n + 1, GLP_FX);
    col_kind.assign(var_n + 1, GLP_CV);
    col_lb.assign(var_n + 1, 0.0);
    col_ub.assign(var_n + 1, 0.0);
    obj.assign(var_n + 1, 0.0);
    return true;
}

void ILP_solver_t::reset() {
    invalid_input = false;
    maximize = false;
    var_n = 0;
    row_type.clear();
    row_lb.clear();
    row_ub.clear();
    col_type.clear();
    col_kind.clear();
    col_lb.clear();
    col_ub.clear();
    obj.clear();
    mat_i.resize(1);
    mat_j.resize(1);
    mat_val.resize(1);
}

void ILP_solver_t::add_row(const int* cols, const double* values,
                           std::size_t n, int type, double lb, double ub) {
    const int row = static_cast<int>(row_type.size()) + 1;
    for (std::size_t k = 0; k < n; ++k) {
        assert(cols[k] >= 1 && cols[k] <= var_n);
        mat_i.push_back(row);
        mat_j.push_back(cols[k]);
        mat_val.push_back(values[k]);
    }
    row_type.push_back(type);
    row_lb.push_back(lb);
    row_ub.push_back(ub);
}

void ILP_solver_t::add_constraint_upb(std::initializer_list<int> cols,
                                      std::initializer_list<double> values,
                                      double upb) {
    assert(cols.size() == values.size());
    add_row(cols.begin(), values.begin(), cols.size(), GLP_UP, 0.0, upb);
}

void ILP_solver_t::add_constraint_upb(const std::vector<int>& cols,
                                      const std::vector<double>& values,
                                      double upb) {
    assert(cols.size() == values.size());
    add_row(cols.data(), values.data(), cols.size(), GLP_UP, 0.0, upb);
}

void ILP_solver_t::add_constraint_fx(std::initializer_list<int> cols,
                                     std::initializer_list<double> values,
                                     double fix) {
    assert(cols.size() == values.size());
    add_row(cols.begin(), values.begin(), cols.size(), GLP_FX, fix, fix);
}

void ILP_solver_t::add_constraint_fx(const std::vector<int>& cols,
                                     const std::vector<double>& values,
                                     double fix) {
    assert(cols.size() == values.size());
    add_row(cols.data(), values.data(), cols.size(), GLP_FX, fix, fix);
}

void ILP_solver_t::set_variable_double_range(int var_i, double lb, double ub) {
    col_type[var_i] = (lb == ub) ? GLP_FX : GLP_DB;
    col_lb[var_i] = lb;
    col_ub[var_i] = ub;
}

void ILP_solver_t::set_variable_double_range_int(int var_i, double lb,
                                                 double ub) {
    set_variable_double_range(var_i, lb, ub);
    col_kind[var_i] = GLP_IV;
}

void ILP_solver_t::set_obj_coef(const std::vector<double>& coef) {
    assert(static_cast<int>(coef.size()) > var_n);
    for (int i = 1; i <= var_n; ++i) {
        obj[i] = coef[i];
    }
}

ILP_result_t ILP_solver_t::solve(bool int_needed) {
    ILP_result_t result;
    if (invalid_input) {
        return result;
    }

    const int row_n = static_cast<int>(row_type.size());
    glp_prob* lp = glp_create_prob();
    glp_set_obj_dir(lp, maximize ? GLP_MAX : GLP_MIN);
    glp_add_rows(lp, row_n > 0 ? row_n : 1);  // GLPK rejects adding 0 rows
    glp_add_cols(lp, var_n);
    for (int r = 1; r <= row_n; ++r) {
        glp_set_row_bnds(lp, r, row_type[r - 1], row_lb[r - 1], row_ub[r - 1]);
    }
    for (int c = 1; c <= var_n; ++c) {
        glp_set_col_bnds(lp, c, col_type[c], col_lb[c], col_ub[c]);
        if (col_kind[c] != GLP_CV) {
            glp_set_col_kind(lp, c, col_kind[c]);
        }
        glp_set_obj_coef(lp, c, obj[c]);
    }
    glp_load_matrix(lp, static_cast<int>(mat_i.size()) - 1, mat_i.data(),
                    mat_j.data(), mat_val.data());

    int status;
    if (int_needed) {
        glp_iocp parm;
        glp_init_iocp(&parm);
        parm.presolve = GLP_ON;
        parm.msg_lev = GLP_MSG_OFF;
        glp_intopt(lp, &parm);
        result.z = glp_mip_obj_val(lp);
        status = glp_mip_status(lp);
    } else {
        glp_smcp parm;
        glp_init_smcp(&parm);
        parm.presolve = GLP_ON;
        parm.msg_lev = GLP_MSG_OFF;
        parm.tm_lim = 5 * 1000;  // ms
        glp_simplex(lp, &parm);
        result.z = glp_get_obj_val(lp);
        status = glp_get_status(lp);
    }

    result.legal = (status == GLP_FEAS || status == GLP_OPT);
    if (result.legal) {
        result.var_values.assign(var_n + 1, 0.0);
        for (int c = 1; c <= var_n; ++c) {
            result.var_values[c] =
                int_needed ? static_cast<int>(glp_mip_col_val(lp, c))
                           : glp_get_col_prim(lp, c);
        }
    }
    glp_delete_prob(lp);
    reset();
    return result;
}
