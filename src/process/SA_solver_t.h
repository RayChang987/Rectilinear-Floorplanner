//
// Created by RayChang on 2023/7/14.
//

#ifndef ICCAD2023PD_SA_SOLVER_T_H
#define ICCAD2023PD_SA_SOLVER_T_H
#include "process/components/timer.h"
#include "sequence_pair/sequence_pair_enumerator_t.h"
#include "sequence_pair/sequence_pair_t.h"

// Simulated annealing over sequence pairs. A move swaps two modules in one or
// both sequences; the cost of a sequence pair is the wirelength predicted by
// the LP. Neighbours are evaluated by several threads in parallel and the
// first accepted neighbour becomes the new state.
class SA_solver_t {
   public:
    // Anneals SPEN.valid_sequence_pairs[0] for `timeout` ms, cooling from
    // init_t to end_t, and stores the best sequence pair found back into it.
    // load_back: jump back to the best solution if the current one is much
    //            worse.
    // overlap:   allow overlapping (interlocked) floorplans.
    void run(sequence_pair_enumerator_t& SPEN, double timeout, double init_t,
             double end_t, bool load_back, bool overlap);

    // Metropolis criterion.
    bool sample_p(double delta_c) const;

    // Relative cost increase of `after` w.r.t. `ori` (x10).
    static double get_delta(const sequence_pair_t& ori,
                            const sequence_pair_t& after);

    // Returns the first accepted neighbour of SP, or SP itself if none.
    sequence_pair_t find_neighbor_parallel(const sequence_pair_t& SP,
                                           bool overlap);

    // Whether to check the routability of every neighbour (slow).
    static constexpr bool need_practical = false;

   private:
    void parameters_init(double init_t, double end_t);
    void update_parameters();
    double get_time_left();

    double t = 1, r = 0.999, end_t = 0.005;
    double time_limit = 0, it_time = 0;
    timer runtime_timer = timer("SA run time");
    timer it_timer = timer("it time");
};

#endif  // ICCAD2023PD_SA_SOLVER_T_H
