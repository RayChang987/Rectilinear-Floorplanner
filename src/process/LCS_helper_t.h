//
// Created by RayChang on 2023/7/22.
//

#ifndef ICCAD2023PD_LCS_HELPER_T_H
#define ICCAD2023PD_LCS_HELPER_T_H

#include "sequence_pair_t.h"
#include <set>
using std::set;
class LCS_helper_t {
public:
    static vector<int> find_LCS_v(sequence_pair_t&, sequence_pair_t&);

    static vector<int> find_LCS_h(sequence_pair_t&, sequence_pair_t&);

    static vector<int> find_LCS(vector<int>&, vector<int>&);

    static set<int> intersect(vector<int>&,vector<int>&);

    static sequence_pair_t filter(sequence_pair_t&, set<int>);

    static sequence_pair_t crossover(sequence_pair_t&, sequence_pair_t&);
};


#endif //ICCAD2023PD_LCS_HELPER_T_H
