//
// Created by RayChang on 2023/6/26.
//

#include "process/sequence_pair/sequence_pair_enumerator_t.h"

#include <iostream>

using std::cout;
using std::endl;

void sequence_pair_enumerator_t::generate_sequence_pairs(int n) {
    init_timer.timer_start();
    target_sp_n = n;
    current_sp_n = 0;
    if (sequence_pair_t::soft_n != 0) {
        if (!add_soft_process_cont(0, false, 0, 0)) {
            cout << "Failed in initialization" << endl;
        }
    } else {
        seed_SP.find_position(true, true);
        valid_sequence_pairs.push_back(seed_SP);
    }
    init_timer.timer_end();
    init_timer.print_time_elapsed();
}

bool sequence_pair_enumerator_t::add_soft_process_cont(int i, bool with_area,
                                                       int start_j,
                                                       int start_k) {
    init_timer.timer_end();
    if (init_timer.get_time_elapsed() >= init_timeout ||
        current_sp_n >= target_sp_n) {
        return false;
    }
    cout << "Initializing-placing " << i << "-th module" << endl;
    if (i >= seed_SP.fix_start_idx) {  // all soft modules are placed
        valid_sequence_pairs.push_back(seed_SP);
        current_sp_n++;
        return true;
    }
    const int module = seed_SP.add_soft_order[i];
    if (seed_SP.is_in_seq[module]) {
        return add_soft_process_cont(i + 1, with_area, start_j, start_k);
    }
    auto place = [&](bool minimize_wirelength) {
        return with_area
                   ? seed_SP.find_position_with_area(false, false)
                   : seed_SP.find_position(minimize_wirelength,
                                           minimize_wirelength);
    };

    auto& v = seed_SP.v_sequence;
    auto& h = seed_SP.h_sequence;
    if (v.empty()) {  // no fixed module: the first module starts the sequences
        v.push_back(module);
        h.push_back(module);
        seed_SP.is_in_seq[module] = 1;
        place(false);
        const bool found = add_soft_process_cont(i + 1, with_area, 0, 0);
        seed_SP.is_in_seq[module] = 0;
        v.pop_back();
        h.pop_back();
        return found;
    }

    const int positions_v = static_cast<int>(v.size()) + 1;
    const int positions_h = static_cast<int>(h.size()) + 1;
    for (int j = 0; j < positions_v; ++j) {
        for (int k = 0; k < positions_h; ++k) {
            const int jj = (j + start_j) % positions_v;
            const int kk = (k + start_k) % positions_h;
            seed_SP.change_size(module);
            v.insert(v.begin() + jj, module);
            h.insert(h.begin() + kk, module);
            seed_SP.is_in_seq[module] = 1;
            if (place(true) &&
                add_soft_process_cont(i + 1, with_area, jj, kk)) {
                return true;
            }
            seed_SP.is_in_seq[module] = 0;
            v.erase(v.begin() + jj);
            h.erase(h.begin() + kk);
            if (current_sp_n >= target_sp_n) {
                return false;
            }
        }
    }
    return false;
}

void sequence_pair_enumerator_t::updated_best_SP() {
    if (valid_sequence_pairs.empty()) {
        return;
    }
    valid_sequence_pairs[0].get_wirelength();
    best_SP = valid_sequence_pairs[0];
}
