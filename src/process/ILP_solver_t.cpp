//
// Created by RayChang on 2023/5/18.
//

#include "ILP_solver_t.h"
#include "utilities/vec2d_t.h"

#include <iostream>
using std::cout;
using std::endl;
ILP_solver_t::ILP_solver_t(){
}
void ILP_solver_t::init(string name, int constraints_n, int variable_n){
//    if(constraints_n==0||variable_n==0){
//        this->invalid_input = true;
//        return;
//    }
    if(variable_n==0){
        this->invalid_input = true;
        return;
    }
    this->constraints_n = constraints_n;
    this->ILP = glp_create_prob();
    glp_set_prob_name(this->ILP, name.c_str());

    glp_add_rows(this->ILP, constraints_n+1);
    glp_add_cols(this->ILP, variable_n);
    var_n = variable_n;

    set_i.resize(1); set_j.resize(1); set_val.resize(1); //due to 1-index :(
}
void ILP_solver_t::set_max() {
    if(invalid_input){return;}
    glp_set_obj_dir(ILP, GLP_MAX);
}

void ILP_solver_t::set_min() {
    if(invalid_input){return;}
    glp_set_obj_dir(ILP, GLP_MIN);
}

void ILP_solver_t::set_constraint_upb(int row_i, int variable_n, vector<int> variable_i, vector<double> values, string name,double upb) {
    for(int i = 0; i<variable_n; ++i){
        set_i.push_back(row_i);
        set_j.push_back(variable_i[i]); //due to sequence# to coefficient#
        set_val.push_back(values[i]);
        set_n++;
    }
    glp_set_row_name(ILP, constraint_i, name.c_str());
    glp_set_row_bnds(ILP, constraint_i, GLP_UP, 0, static_cast<double>(upb)); //x_i-x_j<=- w

    this->constraint_i++;
}

void ILP_solver_t::set_constraint_fx(int row_i, int variable_n, vector<int> variable_i, vector<double> values, string name, double fix) {
    for(int i = 0; i<variable_n; ++i){
        set_i.push_back(row_i);
        set_j.push_back(variable_i[i]); //due to sequence# to coefficient#
        set_val.push_back(values[i]);
        set_n++;
    }
    glp_set_row_name(ILP, constraint_i, name.c_str());
    glp_set_row_bnds(ILP, constraint_i, GLP_FX, fix, fix); //x_i-x_j<=- w

    this->constraint_i++;
}

void ILP_solver_t::set_variable_double_range(int var_i, int lb, double ub) {
    //glp_set_col_bnds(ILP, i, GLP_LO, 0.0, inf);
    if(lb==ub){
        glp_set_col_bnds(ILP, var_i, GLP_FX, lb, ub);
    }
    else{
        glp_set_col_bnds(ILP, var_i, GLP_DB, lb, ub);
    }


}

void ILP_solver_t::set_variable_double_range_int(int var_i, int lb, double ub) {
    //glp_set_col_bnds(ILP, i, GLP_LO, 0.0, inf);
    if(lb==ub){
        glp_set_col_bnds(ILP, var_i, GLP_FX, lb, ub);
    }
    else{
        glp_set_col_bnds(ILP, var_i, GLP_DB, lb, ub);
    }

    glp_set_col_kind(ILP, var_i, GLP_IV);
}
void ILP_solver_t::set_variable_BV(int var_i){
    glp_set_col_kind(ILP, var_i, GLP_BV);
}
void ILP_solver_t::set_obj_coef(vector<double> coef) {
    for(int i = 1; i<=this->var_n; ++i){
        glp_set_obj_coef(this->ILP, i, coef[i]);
    }

}

ILP_result_t ILP_solver_t::solve(bool int_needed) {
    int feasible,z;
    if(int_needed){
        glp_iocp parm;
        glp_init_iocp(&parm);
        parm.presolve = GLP_ON;
        parm.msg_lev = GLP_MSG_OFF;
        int err = glp_intopt(this->ILP, &parm);
        z = glp_mip_obj_val(this->ILP);
        feasible = glp_mip_status(this->ILP);
    }
    else{
        glp_smcp parm;
        glp_init_smcp(&parm);
        parm.presolve = GLP_ON;
        parm.msg_lev = GLP_MSG_OFF;
        parm.tm_lim = 5*1000;
        glp_simplex(this->ILP, &parm);
        z = glp_get_obj_val(this->ILP);
        feasible = glp_get_status(this->ILP);
    }




//    glp_iptcp parm;
//    glp_init_iptcp(&parm);
//    //parm.msg_lev = GLP_MSG_OFF;
//    glp_interior(this->ILP, &parm);
//    int z = glp_ipt_obj_val(this->ILP);
//    int feasible = glp_ipt_status(this->ILP);

    vector<double> result(1); //due to 1-index
    for(int i = 1; i<=this->var_n; ++i){

        if(int_needed){
            result.push_back(static_cast<int>(glp_mip_col_val(this->ILP, i)));
        }
        else{
            result.push_back((glp_get_col_prim(this->ILP, i)));
        }

        //result.push_back(static_cast<int>(glp_ipt_col_prim(this->ILP, i)));
    }
    ILP_result_t ILP_result;
    if(feasible==GLP_FEAS||feasible==GLP_OPT){
        ILP_result.legal = true;
        ILP_result.var_values = result;
        ILP_result.z = z;

    }
    else{
        ILP_result.legal = false;
    }
    release_solver();
    return ILP_result;
}

void ILP_solver_t::load() {
    set_i_a = new int[set_n+1];
    set_j_a = new int[set_n+1];
    set_val_a = new double[set_n+1];
    for(int i = 1; i<=set_n; ++i){
        set_i_a[i] = this->set_i[i];
        set_j_a[i] = this->set_j[i];
        set_val_a[i] = this->set_val[i];
    }
    glp_load_matrix(this->ILP, set_n, set_i_a, set_j_a, set_val_a);
}

void ILP_solver_t::release_solver() {
    delete[] set_i_a;
    delete[] set_j_a;
    delete[] set_val_a;
    glp_delete_prob(this->ILP);
}

bool ILP_solver_t::get_is_invalid() {
    return this->invalid_input;
}





