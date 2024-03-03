//
// Created by RayChang on 2023/6/26.
//

#ifndef ICCAD2023PD_SEQUENCE_PAIR_ENUMERATOR_T_H
#define ICCAD2023PD_SEQUENCE_PAIR_ENUMERATOR_T_H

#include "sequence_pair_t.h"
#include "timer.h"
#include <set>
using std::set;
class sequence_pair_enumerator_t{

public:
    sequence_pair_enumerator_t();

    void generate_sequence_pairs(int n);

    bool add_soft_process(int, bool,int);

    bool add_soft_process_cont(int i,bool with_area, int cutoff, int start_j, int start_k);

    void validate_all_SP();

    void validate_all_SP_print_all();

    void updated_best_SP();

    void load_specific();

    vector<sequence_pair_t> valid_sequence_pairs;

    sequence_pair_t seed_SP, res_SP, best_SP;
    int target_sp_n = 0, current_sp_n = 0;
    double init_timeout, SA_timeout;
    timer init_timer = timer("init timer");


};

#endif //ICCAD2023PD_SEQUENCE_PAIR_ENUMERATOR_T_H
