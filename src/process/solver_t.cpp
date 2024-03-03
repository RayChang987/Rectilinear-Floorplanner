//
// Created by 林士傑 on 2023/3/27.
//

#include "solver_t.h"
//for debug
#include "iostream"
#include <thread>
#include "genetic_solver_t.h"
#include <iomanip>
#include "SA_solver_t.h"
#include "sequence_pair_enumerator_t.h"
#include "LCS_helper_t.h"
#include "random_helper.h"
#include "case_table_t.h"
#include <iomanip>
using std::cout;
using std::endl;
using std::setw;


floorplan_t& solver_t::get_best_fp() {
    return best_fp;
}

solver_t::solver_t() {
    random_helper::set_seed(); //build up random table

    floorplan_t::init(); //class initialization

    sequence_pair_t::init(); //class initialization

    std::ios_base::sync_with_stdio(false); //make cout faster
    std::cin.tie(0);
    cout.tie(0);

    if(chip_t::get_total_module_n()<1){
        this->invalid_input = true;
        return;
    }
}
void solver_t::SA_process(sequence_pair_enumerator_t& SPEN) {
    cout<<"--------------SA process--------------"<<endl;
    if(SPEN.valid_sequence_pairs.size()<1){
        cout<< "Failure in initialization. "<<endl;
        return;
    }
    SA_solver_t SA_solver;
    double time_left = std::min(this->get_time_left(), this->SA_runtime);
    cout<<"---------------Stage 1----------------"<<endl;
    SA_solver.run(SPEN, 0.3 * time_left, 0.5, 0.01, false, 0, 0.33);
    cout<<"---------------Stage 2----------------"<<endl;
    SA_solver.run(SPEN, 0.5 * time_left, 0.1, 0.01, true, 0, 0.66);
    cout<<"---------------Stage 3----------------"<<endl;
    SA_solver.run(SPEN, 0.2 * time_left, 0.03, 0.008, true, 0, 1);
    SPEN.updated_best_SP();

    SPEN.best_SP.find_position(true, true, 0, 0, 9);
    SPEN.best_SP.find_position_with_area(true, true, 0, 0);

    SPEN.best_SP = SA_solver.post_process(SPEN.best_SP);

    SPEN.best_SP.write_inline();
    SPEN.best_SP.print_inline();
    this->best_fp = SPEN.best_SP.to_fp();
    //this->best_fp.GUI_validation();
    cout<<"SA finally got wirelength = "<<std::setprecision(16)<<this->best_fp.get_wirelength()<<endl;
}

void solver_t::run() {
    set_timer_start();

    if(this->invalid_input){
        cout<<"The process was terminated due to invalid inputs"<<endl;
        return;
    }

    sequence_pair_enumerator_t SPEN;
    SPEN.init_timeout = this->init_timeout;
    SPEN.generate_sequence_pairs(1);
    for(auto& e:SPEN.valid_sequence_pairs){double useless = e.get_wirelength(true, true);}

    cout<<"initial stage got = "<<SPEN.valid_sequence_pairs.size()<<" SPs"<<endl;
    cout<<"--------------------------------------"<<endl;
    this->SA_process(SPEN);
    cout<<"----------------"<<endl;
    cout<<"Load specific sequence pair"<<endl;
    this->load_specific_best();
    cout<<"Result : "<<this->best_fp.get_wirelength()<<endl;
    this->runtime_timer.timer_end();
    this->runtime_timer.print_time_elapsed();
}

void solver_t::set_timer_start() {
    this->runtime_timer.timer_start();
}

double solver_t::get_time_left() {
    this->runtime_timer.timer_end();
    double current_time = this->runtime_timer.get_time_elapsed();
    return runtime-current_time;
}

void solver_t::load_specific_best() {
    SA_solver_t SA_solver;
    case_table_t case_table;
    int case_id = chip_t::get_similar_case_num();
    sequence_pair_t SP;
    bool fnd_cases = false;
    if(case_id!=-1){
        fnd_cases = true;
        SP.v_sequence = case_table.cases[case_id][0];
        SP.h_sequence = case_table.cases[case_id][1];
        for(auto& e:SP.is_in_seq){e = 1;}
    }
    if(fnd_cases){
        SP.find_position(true, true, 0, 0,9);
        SP.find_position_with_area(true, true, 0, 0);
        SP = SA_solver.post_process(SP);
        floorplan_t loaded_fp = SP.to_fp();
        if(loaded_fp.get_wirelength()<this->best_fp.get_wirelength()){
            this->best_fp = loaded_fp;
            cout<<"Apply the prepared SP..."<<endl;
        }
    }

}

void solver_t::load_specific_without_cmp() {
    SA_solver_t SA_solver;
    case_table_t case_table;
    int case_id = chip_t::get_similar_case_num();
    sequence_pair_t SP;
    bool fnd_cases = false;
    if(case_id!=-1){
        fnd_cases = true;
        SP.v_sequence = case_table.cases[case_id][0];
        SP.h_sequence = case_table.cases[case_id][1];
        for(auto& e:SP.is_in_seq){e = 1;}
    }
    if(fnd_cases){
        SP.find_position(true, true, 0, 0, 9);
        SP.find_position_with_area(true, true, 0, 0);
        SP = SA_solver.post_process(SP);
        SP.print_inline();
        //t1.print_time_elapsed();
        floorplan_t loaded_fp = SP.to_fp();
        this->best_fp = loaded_fp;
        cout<< "Result : "<<std::setprecision(16)<<this->best_fp.get_wirelength()<<endl;
    }
}

void solver_t::test() {
    for(auto& e:sequence_pair_t::deg_w){
        cout<<e.first<<" "<<e.second<<endl;
    }
}



