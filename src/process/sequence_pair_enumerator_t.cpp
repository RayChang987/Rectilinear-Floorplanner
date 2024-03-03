//
// Created by RayChang on 2023/6/26.
//

#include "sequence_pair_enumerator_t.h"
#include <iostream>
#include <iomanip>
#include "random_helper.h"
using std::cout;
using std::endl;
sequence_pair_enumerator_t::sequence_pair_enumerator_t(){
    this->seed_SP = sequence_pair_t();
}

void sequence_pair_enumerator_t::generate_sequence_pairs(int n) {
    this->init_timer.timer_start();
    this->target_sp_n = n;
    this->current_sp_n = 0;
    if(sequence_pair_t::soft_n!=0){
        bool result  = this->add_soft_process_cont(0, false, 5, 0, 0);
    }
    this->init_timer.timer_end();
    this->init_timer.print_time_elapsed();
}

bool sequence_pair_enumerator_t::add_soft_process(int i, bool with_area, int cutoff) {
    if(this->valid_sequence_pairs.size()>=target_sp_n){
        return false;
    }
    if(i>=this->seed_SP.fix_start_idx){
        this->seed_SP.predict_wirelength(true, with_area); //minimize wirelength at last
        if(this->seed_SP.predicted_wirelength!=-1){
            this->valid_sequence_pairs.push_back(this->seed_SP);
            return true;
        }
        else{
            return false;
        }
    }
    bool fnd = false;
    if(seed_SP.v_sequence.size() == 0){ //in case there are no fix module
        seed_SP.v_sequence.push_back(this->seed_SP.add_soft_order[i]);
        seed_SP.h_sequence.push_back(this->seed_SP.add_soft_order[i]);
        this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]] = 1;
        bool success;
        if(with_area){
            success = this->seed_SP.find_position_with_area(false, false, 0, 0);
        }
        else{
            success = this->seed_SP.find_position(false, false, 0, 0);
        }
        fnd|= this->add_soft_process(i+1,with_area, cutoff);
        this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]] = 0;
        seed_SP.v_sequence.pop_back();
        seed_SP.h_sequence.pop_back();
        return fnd;
    }

    int success_n = 0;

    for(int j = 0; j <= seed_SP.v_sequence.size(); ++j){
        for(int k = 0; k <= seed_SP.h_sequence.size(); ++k){
            seed_SP.change_size(i);
            seed_SP.v_sequence.insert(seed_SP.v_sequence.begin() + j, this->seed_SP.add_soft_order[i]);
            seed_SP.h_sequence.insert(seed_SP.h_sequence.begin() + k, this->seed_SP.add_soft_order[i]);
            this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]] = 1;
            bool success;
            if(with_area){
                //cout<< i<<" : ";
                timer a1("find time with area");
                a1.timer_start();
                success = this->seed_SP.find_position_with_area(false, false, 0, 0);
                a1.timer_end();
                //a1.print_time_elapsed();
            }
            else{
                //cout<< i<<" : ";
                timer a1("find time");
                a1.timer_start();
                success = this->seed_SP.find_position(false, false, 0, 0);
                a1.timer_end();
                //a1.print_time_elapsed();
            }
            if(success){
                if(this->add_soft_process(i+1, with_area, cutoff)){
                    fnd|=1;
                }
            }
            this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]] = 0;
            seed_SP.v_sequence.erase(seed_SP.v_sequence.begin() + j);
            seed_SP.h_sequence.erase(seed_SP.h_sequence.begin() + k);
            if(fnd&&i>=cutoff){
                return true;
            }
        }
    }
    //cout<<i<<" : success n  = "<<success_n<<endl;
    return false;
}
bool sequence_pair_enumerator_t::add_soft_process_cont(int i,bool with_area, int cutoff, int start_j, int start_k) {
    this->init_timer.timer_end();
    if( this->init_timer.get_time_elapsed() >= this->init_timeout){
        return false;
    }
    if(this->current_sp_n >= this->target_sp_n){
        return false;
    }
    if(i>=this->seed_SP.fix_start_idx){
        this->seed_SP.predict_wirelength(true, false); //minimize wirelength at last
        this->res_SP = this->seed_SP;
        if(this->seed_SP.predicted_wirelength!=-1){
            this->valid_sequence_pairs.push_back(this->seed_SP);
        }
        this->current_sp_n++;
        //this->seed_SP.print_inline();
        return true;
    }
    if(this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]]){
        return add_soft_process_cont(i+1, with_area, cutoff, start_j, start_k);
    }

    bool fnd = false;
    if(seed_SP.v_sequence.size() == 0){ //in case there are no fix module
        seed_SP.v_sequence.push_back(this->seed_SP.add_soft_order[i]);
        seed_SP.h_sequence.push_back(this->seed_SP.add_soft_order[i]);
        this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]] = 1;
        bool success;
        if(with_area){
            success = this->seed_SP.find_position_with_area(false, false, 0, 0);
        }
        else{
            success = this->seed_SP.find_position(false, false, 0, 0);
        }
        fnd|= this->add_soft_process_cont(i+1,with_area, cutoff, 0, 0);
        this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]] = 0;
        seed_SP.v_sequence.pop_back();
        seed_SP.h_sequence.pop_back();
        return fnd;
    }
    for(int j = 0; j <= seed_SP.v_sequence.size(); ++j){
        for(int k = 0; k <= seed_SP.h_sequence.size(); ++k){
            int jj = (j+start_j)%(seed_SP.v_sequence.size()+1);
            int kk = (k+start_k)%(seed_SP.h_sequence.size()+1);
            seed_SP.change_size(this->seed_SP.add_soft_order[i]);

            seed_SP.v_sequence.insert(seed_SP.v_sequence.begin() + jj, this->seed_SP.add_soft_order[i]);
            seed_SP.h_sequence.insert(seed_SP.h_sequence.begin() + kk, this->seed_SP.add_soft_order[i]);
            this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]] = 1;
            bool success;
            if(with_area){
                success = this->seed_SP.find_position_with_area(false, false, 0, 0);
            }
            else{
                success = this->seed_SP.find_position(false, false, 0, 0);
            }
            if(success){
                if(this->add_soft_process_cont(i+1, with_area, cutoff, jj, kk)){
                    fnd|=1;
                }
            }
            this->seed_SP.is_in_seq[this->seed_SP.add_soft_order[i]] = 0;
            seed_SP.v_sequence.erase(seed_SP.v_sequence.begin() + jj);
            seed_SP.h_sequence.erase(seed_SP.h_sequence.begin() + kk);
            if(fnd&&i>=cutoff){
                return true;
            }
            if(this->current_sp_n>=this->target_sp_n){
                return false;
            }
        }
    }
    return false;
}
void sequence_pair_enumerator_t::validate_all_SP() {
    cout<< "currently got "<< this->valid_sequence_pairs.size()<<" sequences pairs"<<endl;
    for(int i = 0; i<this->valid_sequence_pairs.size(); ++i){
        bool valid = this->valid_sequence_pairs[i].find_position_with_area(false,false, 0, 0);
        if(valid==false){
            cout<<"invalid SP"<<endl;
        }
        else{
            double wirelength = this->valid_sequence_pairs[i].get_wirelength(true, true);
            cout<<"valid SP : "<<std::setprecision(16)<<wirelength<<endl;

        }
    }
}


void sequence_pair_enumerator_t::validate_all_SP_print_all() {
    cout<< "currently got "<< this->valid_sequence_pairs.size()<<" sequences pairs"<<endl;
    for(int i = 0; i<this->valid_sequence_pairs.size(); ++i){
        bool valid = this->valid_sequence_pairs[i].find_position_with_area(false,false, 0, 0);
        if(valid==false){
            cout<<"invalid SP"<<endl;
        }
        else{
            cout<<"valid SP "<<endl;
        }
        this->valid_sequence_pairs[i].print_inline();
    }
}
void sequence_pair_enumerator_t::updated_best_SP() {
    if(this->valid_sequence_pairs.size()<1){
        return;
    }
    double current_best_wirelength = this->valid_sequence_pairs[0].get_wirelength(true, true);
    this->best_SP = this->valid_sequence_pairs[0];
    for(auto SP:this->valid_sequence_pairs){
        double v = SP.get_wirelength(true, true);
        if(v < current_best_wirelength){
            this->best_SP = SP;
            current_best_wirelength = v;
        }
    }
}

void sequence_pair_enumerator_t::load_specific() {
    sequence_pair_t SP;
    //load the full scale test case
    SP.v_sequence = {113, 34, 112, 84, 27, 67, 111, 76, 110, 50, 94, 99, 141, 140, 139, 138, 100, 32, 31, 83, 30, 29, 82, 80, 93, 28, 33, 142, 156, 170, 184, 109, 22, 128, 36, 61, 60, 62, 59, 23, 54, 137, 101, 5, 55, 58, 21, 57, 143, 157, 171, 185, 144, 158, 172, 186, 145, 159, 173, 187, 146, 160, 174, 188, 147, 161, 175, 189, 53, 148, 52, 162, 176, 190, 108, 18, 129, 98, 1, 19, 86, 20, 56, 26, 136, 102, 16, 2, 3, 37, 4, 150, 151, 152, 153, 154, 155, 149, 164, 165, 166, 167, 168, 169, 163, 178, 179, 180, 181, 182, 183, 177, 192, 193, 194, 195, 196, 197, 191, 63, 97, 77, 49, 198, 199, 6, 114, 85, 87, 78, 0, 115, 65, 64, 116, 68, 91, 66, 69, 107, 130, 79, 15, 25, 81, 135, 103, 127, 41, 12, 96, 44, 92, 75, 74, 73, 72, 43, 71, 70, 90, 95, 117, 24, 104, 126, 48, 39, 38, 11, 17, 13, 45, 40, 35, 14, 42, 118, 105, 125, 8, 51, 10, 7, 47, 46, 9, 119, 106, 131, 132, 133, 134, 124, 122, 89, 88, 123, 121, 120};
    SP.h_sequence = {138, 139, 140, 141, 137, 54, 33, 62, 28, 99, 94, 23, 61, 93, 136, 36, 80, 26, 59, 86, 56, 60, 19, 20, 98, 1, 135, 81, 25, 134, 88, 133, 132, 89, 131, 128, 129, 130, 110, 50, 76, 111, 67, 112, 27, 113, 109, 22, 82, 84, 30, 29, 34, 32, 31, 184, 170, 156, 142, 108, 52, 18, 53, 185, 21, 186, 187, 188, 189, 190, 171, 172, 173, 174, 83, 175, 176, 157, 158, 159, 160, 161, 162, 143, 144, 58, 145, 146, 147, 57, 5, 148, 107, 79, 15, 85, 87, 6, 63, 97, 49, 199, 198, 55, 191, 192, 193, 194, 195, 77, 196, 197, 177, 178, 179, 180, 181, 182, 183, 163, 164, 165, 166, 167, 168, 169, 149, 150, 151, 152, 2, 153, 16, 154, 155, 37, 122, 123, 121, 106, 124, 8, 51, 47, 46, 120, 105, 125, 10, 7, 48, 39, 9, 38, 119, 11, 17, 45, 14, 104, 126, 96, 13, 42, 92, 75, 74, 73, 40, 72, 71, 70, 35, 118, 103, 127, 3, 41, 12, 44, 43, 90, 4, 0, 24, 78, 65, 95, 64, 68, 91, 66, 117, 69, 100, 101, 102, 114, 115, 116};
    this->valid_sequence_pairs.push_back(SP);
}



