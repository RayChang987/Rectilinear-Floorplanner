//
// Created by 林士傑 on 2023/3/27.
//

#ifndef INC_2023PD_SOLVER_T_H
#define INC_2023PD_SOLVER_T_H

#include "process/components/timer.h"
#include "process/sequence_pair/sequence_pair_enumerator_t.h"

// The floorplanning flow: initial sequence pair -> simulated annealing ->
// interlocking (rectilinear floorplan) -> checking & plotting.
class solver_t {
   public:
    solver_t();

    void run();

    void SA_process(sequence_pair_enumerator_t& SPEN);

    double get_time_left();  // ms

    void set_timer_start();

    double runtime = 40 * 60 * 1000;     // ms, whole flow
    double SA_runtime = 1 * 60 * 1000;   // ms, simulated annealing
    double init_timeout = 2 * 60 * 1000;  // ms, initial sequence pair
    bool invalid_input = false;
    timer runtime_timer = timer("run time");
};

#endif  // INC_2023PD_SOLVER_T_H
