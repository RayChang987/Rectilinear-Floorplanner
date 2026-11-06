//
// Created by 林士傑 on 2023/3/27.
//

#include "solver_t.h"

#include <algorithm>
#include <iostream>

#include "SA_solver_t.h"
#include "static_data/chip_t.h"
#include "static_data/io_handler_t.h"

using std::cout;
using std::endl;

solver_t::solver_t() {
    sequence_pair_t::init();  // class initialization

    std::ios_base::sync_with_stdio(false);  // make cout faster
    std::cin.tie(nullptr);

    if (chip_t::get_total_module_n() < 1) {
        invalid_input = true;
    }
}

void solver_t::run() {
    set_timer_start();
    if (invalid_input) {
        cout << "The process was terminated due to invalid inputs" << endl;
        return;
    }

    sequence_pair_enumerator_t SPEN;
    SPEN.init_timeout = init_timeout;
    SPEN.generate_sequence_pairs(1);
    if (SPEN.valid_sequence_pairs.empty()) {
        cout << "Failure in initialization. " << endl;
        return;
    }
    SPEN.valid_sequence_pairs[0].sequence_pair_validation();
    SPEN.valid_sequence_pairs[0].print_inline();
    SPEN.valid_sequence_pairs[0].get_wirelength();
    SA_process(SPEN);
}

void solver_t::SA_process(sequence_pair_enumerator_t& SPEN) {
    cout << "--------------SA process--------------" << endl;
    if (SPEN.valid_sequence_pairs.empty()) {
        cout << "Failure in initialization. " << endl;
        return;
    }
    // A long annealing followed by a short re-heated one.
    SA_solver_t SA_solver;
    const double time_left = std::min(get_time_left(), SA_runtime);
    SA_solver.run(SPEN, 0.8 * time_left, 0.05, 0.008, false, true);
    SA_solver.run(SPEN, 0.2 * time_left, 0.05, 0.008, false, true);

    SPEN.updated_best_SP();
    sequence_pair_t& best = SPEN.valid_sequence_pairs[0];
    best.print_inline();
    best.sequence_pair_validation(1);
    best.to_rectilinear();
    const size_t illegal_n = best.get_illegal_modules().size();
    if (illegal_n > 0) {
        const bool legal = best.legalize_rectilinear();
        logger_t::info({"Repaired " + std::to_string(illegal_n) + " illegal module(s): " +
                        std::to_string(best.get_illegal_modules().size()) + " left" +
                        (legal ? "" : " (could not be repaired)")});
    }
    best.plot_rectilinear();
    best.check_rectilinear_result(true);
}

void solver_t::set_timer_start() { runtime_timer.timer_start(); }

double solver_t::get_time_left() {
    runtime_timer.timer_end();
    return runtime - runtime_timer.get_time_elapsed();
}
