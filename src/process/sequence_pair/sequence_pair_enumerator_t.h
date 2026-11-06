//
// Created by RayChang on 2023/6/26.
//

#ifndef ICCAD2023PD_SEQUENCE_PAIR_ENUMERATOR_T_H
#define ICCAD2023PD_SEQUENCE_PAIR_ENUMERATOR_T_H

#include "process/components/timer.h"
#include "sequence_pair_t.h"

// Builds initial (legal) sequence pairs: starting from the fixed modules, the
// soft modules are inserted one by one (largest first) at the first position
// where the LP is still feasible, backtracking if necessary.
class sequence_pair_enumerator_t {
   public:
    // Searches for up to n legal sequence pairs (valid_sequence_pairs).
    void generate_sequence_pairs(int n);

    // Inserts the i-th soft module (in seed_SP.add_soft_order) and recurses.
    // The search for the insertion position starts at (start_j, start_k).
    bool add_soft_process_cont(int i, bool with_area, int start_j, int start_k);

    // best_SP = valid_sequence_pairs[0]
    void updated_best_SP();

    vector<sequence_pair_t> valid_sequence_pairs;
    sequence_pair_t seed_SP, best_SP;
    int target_sp_n = 0, current_sp_n = 0;
    double init_timeout = 0;  // ms
    timer init_timer = timer("init timer");
};

#endif  // ICCAD2023PD_SEQUENCE_PAIR_ENUMERATOR_T_H
