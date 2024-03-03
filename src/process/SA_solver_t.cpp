//
// Created by RayChang on 2023/7/14.
//

#include "SA_solver_t.h"
#include "random_helper.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include "timer.h"
#include <thread>
#include <stack>
#include <mutex>
std::stack<sequence_pair_t> legal_neighbors;
std::mutex legal_neighbors_mutex;
using std::cout;
using std::endl;
bool SA_solver_t::sample_p(double delta_c) {
    double p = exp(-delta_c/t);
    return random_helper::sample(p);
}
SA_solver_t::SA_solver_t() {
}
void SA_solver_t::run(sequence_pair_enumerator_t & SPEN, double timeout, double init_t, double end_t, bool load_back, double enable_from, double enable_to) {
    this->swap_enable = vector<int>(sequence_pair_t::sequence_n, 0);
    for(int i = static_cast<int>(sequence_pair_t::sequence_n * enable_from); i<static_cast<int>(sequence_pair_t::sequence_n * enable_to); ++i){
        this->swap_enable[i] = 1;
    }
    this->parameters_init(init_t, end_t);
    this->runtime_timer.timer_start();
    this->time_limit = timeout;

    SPEN.updated_best_SP();
    sequence_pair_t best_sp = SPEN.best_SP;
    int load_back_cnt = 0;
    int it = 1;
    while(true){
        runtime_timer.timer_end();
        if(runtime_timer.get_time_elapsed() >= timeout){break;}

        this->it_timer.timer_start();
        sequence_pair_t& SP = SPEN.valid_sequence_pairs[0];
        //sequence_pair_t after = find_neighbor_parallel(SP);
        sequence_pair_t after = find_neighbor_sequential(SP);
        if(after.predicted_wirelength!=-1){
            SP = after;
        }
        if(SP.predicted_wirelength < best_sp.predicted_wirelength && SP.predicted_wirelength!=-1){
            best_sp = SP;
            load_back_cnt = 0;
        }
        else{
            load_back_cnt++;
            if(load_back && SP.predicted_wirelength>1.375*best_sp.predicted_wirelength){
                cout<<"Load back..."<<endl;
                SP = best_sp; //to avoid meaningless searching
                load_back_cnt = 0;
            }
        }
//        if(it%100==0){
//            double useless1 = best_sp.get_wirelength(true, true);
//            double useless2 = SPEN.valid_sequence_pairs[0].get_wirelength(true, true);
//        }
        if(it%1==0){
            runtime_timer.print_time_elapsed();
            cout<<"It : "<<it<<", t = "<<this->t<<endl;
            this->it_timer.timer_end();
            this->it_timer.print_time_elapsed();
            cout<<"current best wirelength : "<<std::setprecision(16)<<best_sp.predicted_wirelength<<endl;
            cout<<"current wirelength : "<<std::setprecision(16)<<SPEN.valid_sequence_pairs[0].predicted_wirelength<<endl;
            cout<<"------------------------------"<<endl;
        }
        runtime_timer.timer_end();
        if(runtime_timer.get_time_elapsed() >= timeout){break;}
        this->t*=r;
        this->it_timer.timer_end();
        //this->it_average_time =  (this->it_average_time*(it-1)+this->it_timer.get_time_elapsed()) / it;
        this->it_average_time =  this->it_timer.get_time_elapsed();

        update_parameters();

        it++;
    }
    //SPEN.validate_all_SP_print_all();
    SPEN.valid_sequence_pairs[0] = best_sp; //reload the SP back into sequence pairs
}

void find_neighbor_threads_i(int i_start, int i_end, vector<int>* rand_i, vector<int>* rand_j, vector<int>* h_map, vector<int>* v_map, SA_solver_t* SA, sequence_pair_t);
sequence_pair_t SA_solver_t::find_neighbor_sequential(sequence_pair_t SP){
    vector<int> v_map(sequence_pair_t::sequence_n), h_map(sequence_pair_t::sequence_n);
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        v_map[SP.v_sequence[i]] = h_map[SP.h_sequence[i]] = i;
    }
    sequence_pair_t neighbor = SP;
    vector<int> rand_i = random_helper::rand_list(sequence_pair_t::sequence_n);
    vector<int> rand_j = random_helper::rand_list(sequence_pair_t::sequence_n);
    timer find_neighbor_time("find time");
    find_neighbor_time.timer_start();
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        for(int j = 0; j<sequence_pair_t::sequence_n; ++j){
            int ii = rand_i[i], jj = rand_j[j];
            if(sequence_pair_t::seq_is_fix[ii]&&sequence_pair_t::seq_is_fix[jj]){continue;}
            int p = v_map[ii], q = h_map[jj];
            if(swap_enable[ii]==false&&swap_enable[jj]==false){continue;}
            for(int m = 0; m<2; ++m){
                for(int n = 0; n<2; ++n){
                    if(m==0&&n==0){continue;}
                    if(m){std::swap(neighbor.v_sequence[p], neighbor.v_sequence[q]);}
                    if(n){std::swap(neighbor.h_sequence[p], neighbor.h_sequence[q]);}

                    timer find_position_timer("find position time (in finding neighbor)");
                    find_position_timer.timer_start();
                    bool success = neighbor.find_position(false, true, 0, 0); //6ms at most  (the shapes of the neighbor SP were calculated)
                    find_position_timer.timer_end();
                    //find_position_timer.print_time_elapsed();

                    if(success){
                        double delta = get_delta(SP, neighbor);
                        bool change = sample_p(delta);
                        if(change){
                            find_neighbor_time.timer_end();
                            return neighbor;
                        }
                    }
                    if(m){std::swap(neighbor.v_sequence[p], neighbor.v_sequence[q]);}
                    if(n){std::swap(neighbor.h_sequence[p], neighbor.h_sequence[q]);}
                }
            }

        }
    }
    return SP; //can't find any neighbor
}
sequence_pair_t SA_solver_t::find_neighbor_parallel(sequence_pair_t SP) {
    vector<int> v_map(sequence_pair_t::sequence_n), h_map(sequence_pair_t::sequence_n);
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        v_map[SP.v_sequence[i]] = h_map[SP.h_sequence[i]] = i;
    }
    vector<int> rand_i = random_helper::rand_list(sequence_pair_t::sequence_n);
    vector<int> rand_j = random_helper::rand_list(sequence_pair_t::sequence_n);
    double thread_n;
    if(this->t>=0.1){
        thread_n = 2;
    }
    else{
        thread_n = 4;
    }
    while(legal_neighbors.size()){legal_neighbors.pop();}
    vector<std::thread> threads;
    double x = sequence_pair_t::sequence_n/thread_n;

    for(int i = 0; i<thread_n; ++i){
        threads.push_back(std::thread(find_neighbor_threads_i, i*x, (i+1)*x, &rand_i, &rand_j, &h_map, &v_map, this, SP));
    }
    for(int i = 0; i<thread_n; ++i){
        threads[i].join();
    }
    if(legal_neighbors.empty()){
        return SP;
    }
    else{
        return legal_neighbors.top();
    }
}

double SA_solver_t::get_delta(sequence_pair_t & ori, sequence_pair_t& after) {
    double ori_wirelength = ori.predicted_wirelength;
    double after_wirelength = after.get_wirelength(true, false);
    if(after_wirelength<=0||ori_wirelength<=0){
        return 0;
    }
    double delta = (after_wirelength-ori_wirelength)/ori_wirelength;
    return delta*10.0;
}

void SA_solver_t::parameters_init(double t, double end_t) {
    this->t = t;
    this->r = 0.999;
    this->end_t = end_t;
}

double SA_solver_t::get_time_left() {
    this->runtime_timer.timer_end();
    double current_time = this->runtime_timer.get_time_elapsed();
    return std::max(time_limit - current_time, 0.0);
}

void SA_solver_t::update_parameters() {
    if(this->t<=0){return;}
    double time_left = this->get_time_left();
    double it_time = std::max(this->it_average_time, 0.0000001); //to avoid divide by 0
    double it_left = std::max(time_left/it_time, 1.0);
    double new_r = pow((this->end_t)/this->t, 1/it_left);
    this->r =  new_r;
}

void find_neighbor_threads_i(int i_start, int i_end, vector<int>* rand_i, vector<int>* rand_j, vector<int>* h_map, vector<int>* v_map, SA_solver_t* SA,sequence_pair_t SP){
    sequence_pair_t neighbor = SP;
    for(int i = i_start; i<i_end; ++i){
        for(int j = 0; j<sequence_pair_t::sequence_n; ++j){
            int ii = (*rand_i)[i], jj = (*rand_j)[j];
            if(sequence_pair_t::seq_is_fix[ii]&&sequence_pair_t::seq_is_fix[jj]){continue;}
            int p = (*v_map)[ii], q = (*h_map)[jj];
            for(int m = 0; m<2; ++m){
                for(int n = 0; n<2; ++n){
                    if(m==0&&n==0){continue;}
                    if(legal_neighbors.size()){return;}
                    if(m){std::swap(neighbor.v_sequence[p], neighbor.v_sequence[q]);}
                    if(n){std::swap(neighbor.h_sequence[p], neighbor.h_sequence[q]);}
                    bool success = neighbor.find_position(false, true, 0, 0); //6ms at most  (the shapes of the neighbor SP were calculated)
                    if(success){
                        double delta = SA->get_delta(SP, neighbor);
                        bool change = SA->sample_p(delta);
                        if(change){
                            legal_neighbors_mutex.lock();
                            legal_neighbors.push(neighbor);
                            legal_neighbors_mutex.unlock();
                            return;
                        }
                    }
                    if(m){std::swap(neighbor.v_sequence[p], neighbor.v_sequence[q]);}
                    if(n){std::swap(neighbor.h_sequence[p], neighbor.h_sequence[q]);}

                }
            }

        }
    }
}


sequence_pair_t SA_solver_t::post_process(sequence_pair_t SP) {
    std::vector<rect_t> rects;
    rects = std::vector<rect_t>(chip_t::get_total_module_n(), rect_t(vec2d_t::default_position, vec2d_t::default_position));
    for(auto poly : SP.to_fp().polygon_forest.get_polygons()){
        if(poly.get_rects().size() != 1){
        }
        for(auto po : poly.get_rects()){
            rects[chip_t::get_name_to_index_mapping().at(po->module_rect.getLinkModule()->getName())] = po->module_rect.getRect();
        }
    }
    for (int i = 0; i < chip_t::get_soft_modules().size(); ++i) {
        //carving
        auto limit_area = chip_t::get_modules()[i]->get_area();
        auto minus_rect = rects[i];
        auto new_length = 0;
        if(rects[i].get_size().get_y() > rects[i].get_size().get_x()){
            minus_rect = rect_t(rects[i].get_left_lower(), rects[i].get_size() - vec2d_t(1, 0));
            while(minus_rect.get_size().get_y() < 2 * minus_rect.get_size().get_x() && minus_rect.get_area() >= limit_area){
                rects[i] = minus_rect;
                minus_rect = rect_t(rects[i].get_left_lower(), rects[i].get_size() - vec2d_t(1, 0));
            }
            new_length = (int)(limit_area / rects[i].get_size().get_x()) + 1;
            rects[i] = rect_t(rects[i].get_left_lower(), vec2d_t(rects[i].get_size().get_x(), new_length));
        }else{
            minus_rect = rect_t(rects[i].get_left_lower(), rects[i].get_size() - vec2d_t(0, 1));
            while(minus_rect.get_size().get_x() < 2 * minus_rect.get_size().get_y() && minus_rect.get_area() >= limit_area){
                rects[i] = minus_rect;
                minus_rect = rect_t(rects[i].get_left_lower(), rects[i].get_size() - vec2d_t(0, 1));
            }
            new_length = (int)(limit_area / rects[i].get_size().get_y()) + 1;
            rects[i] = rect_t(rects[i].get_left_lower(), vec2d_t(new_length, rects[i].get_size().get_y()));
        }
    }

    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        SP.modules_wh[i] = rects[i].get_size();
    }
    sequence_pair_t tmp = SP;
    bool success = tmp.find_position_with_area(true, true, 0, 0);
    if(success){
        return tmp;
    }
    else{
        return SP;
    }

}