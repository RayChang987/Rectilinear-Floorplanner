//
// Created by RayChang on 2023/7/14.
//

#ifndef ICCAD2023PD_SA_SOLVER_T_H
#define ICCAD2023PD_SA_SOLVER_T_H
#include "sequence_pair_t.h"
#include "sequence_pair_enumerator_t.h"
class SA_solver_t {
public:
    SA_solver_t();

    bool sample_p(double delta_c);

    void run(sequence_pair_enumerator_t&,double, double, double,bool, double, double);

    double get_delta(sequence_pair_t&, sequence_pair_t&);

    void parameters_init(double, double);

    void update_parameters();

    double get_time_left();

    sequence_pair_t post_process(sequence_pair_t);

//    void find_greater(sequence_pair_t&, double timeout);

    sequence_pair_t find_neighbor_parallel(sequence_pair_t);

    sequence_pair_t find_neighbor_sequential(sequence_pair_t);


    vector<int> swap_enable;
    double t = 1, r = 0.999, end_t = 0.005;
    double time_limit = 0, it_average_time = 0;
    const int load_back_it = 100;
    timer runtime_timer = timer("SA run time");
    timer it_timer = timer("it time");
};


#endif //ICCAD2023PD_SA_SOLVER_T_H
