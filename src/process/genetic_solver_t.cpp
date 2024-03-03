//
// Created by RayChang on 2023/6/28.
//

#include "genetic_solver_t.h"
#include <algorithm>
#include "timer.h"
#include <iostream>
using std::cout;
using std::endl;

genetic_solver_t::genetic_solver_t(int greater_sequence_n,int max_sequence_n) {
    this->greater_sequence_n = greater_sequence_n;
    this->max_sequences_n = max_sequence_n;
}
void genetic_solver_t::generate_sequence_pair(int n){
}
void genetic_solver_t::sort_sequence_pair(){
    std::sort(this->sequence_pairs.begin(), this->sequence_pairs.end(), [](sequence_pair_t& a, sequence_pair_t& b){
       return a.predicted_wirelength<b.predicted_wirelength;
    });
}
void genetic_solver_t::selection(){
    this->sort_sequence_pair();
    while(this->sequence_pairs.size()>this->greater_sequence_n){
        this->sequence_pairs.pop_back();
    }
}


void genetic_solver_t::update_best_sequence_pair(){
    if(this->sequence_pairs[0].predicted_wirelength<this->best_sequence_pair.predicted_wirelength){
        this->best_sequence_pair = this->sequence_pairs[0];
    }
}

void genetic_solver_t::run() {
}

