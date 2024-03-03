//
// Created by RayChang on 2023/6/28.
//

#ifndef ICCAD2023PD_GENETIC_SOLVER_T_H
#define ICCAD2023PD_GENETIC_SOLVER_T_H
#include "sequence_pair_t.h"
#include "vector"
using std::vector;
class genetic_solver_t{
public:
    genetic_solver_t(int greater_sequence_n, int max_sequence_n);

    void run();

    void generate_sequence_pair(int n);

    void sort_sequence_pair();

    void selection();

    void update_best_sequence_pair();

    vector<sequence_pair_t> sequence_pairs;
    sequence_pair_t best_sequence_pair;
    int max_sequences_n,greater_sequence_n;
};


#endif //ICCAD2023PD_GENETIC_SOLVER_T_H
