//
// Created by RayChang on 2023/7/14.
//

#include "process/SA_solver_t.h"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <optional>
#include <thread>

#include "bounding/bounding_line_handler_t.h"
#include "process/functional/random_helper.h"

using std::cout;
using std::endl;

namespace {

// State shared by the threads of one find_neighbor_parallel() call.
struct neighbor_search_t {
    std::atomic<bool> found{false};
    std::mutex mutex;
    std::optional<sequence_pair_t> result;

    void publish(const sequence_pair_t& neighbor) {
        std::lock_guard<std::mutex> lock(mutex);
        if (!result) {
            result = neighbor;
        }
        found = true;
    }
};

// Evaluates the moves of the modules rand_i[i_start .. i_end).
void search_neighbors(int i_start, int i_end, const vector<int>& rand_i,
                      const vector<int>& rand_j, const vector<int>& v_map,
                      const vector<int>& h_map, const SA_solver_t& SA,
                      const sequence_pair_t& SP, bool overlap,
                      neighbor_search_t& search) {
    // Swap in V only, in H only, or in both sequences.
    constexpr bool swap_modes[3][2] = {{false, true}, {true, false}, {true, true}};
    sequence_pair_t neighbor = SP;
    for (int i = i_start; i < i_end; ++i) {
        for (int j = 0; j < sequence_pair_t::sequence_n; ++j) {
            const int ii = rand_i[i], jj = rand_j[j];
            if (sequence_pair_t::seq_is_fix[ii] &&
                sequence_pair_t::seq_is_fix[jj]) {
                continue;
            }
            const int p = v_map[ii], q = h_map[jj];
            for (const auto& mode : swap_modes) {
                if (search.found) {
                    return;  // another thread already found a neighbour
                }
                const bool swap_v = mode[0], swap_h = mode[1];
                if (swap_v) {
                    std::swap(neighbor.v_sequence[p], neighbor.v_sequence[q]);
                }
                if (swap_h) {
                    std::swap(neighbor.h_sequence[p], neighbor.h_sequence[q]);
                }
                bool success;
                if (overlap) {
                    success = neighbor.find_position_allow_illegal_process();
                    if (success && SA_solver_t::need_practical) {
                        neighbor.bounding_lines.resize(
                            sequence_pair_t::sequence_n);
                        neighbor.carved =
                            vector<int>(sequence_pair_t::sequence_n, false);
                        neighbor.set_bounding_lines();
                        neighbor.deal_bounding_line();
                        success =
                            bounding_line_handler_t::check_sequence_pair(neighbor);
                    }
                } else {
                    success = neighbor.find_position(true, true);
                }
                if (success &&
                    SA.sample_p(SA_solver_t::get_delta(SP, neighbor))) {
                    search.publish(neighbor);
                    return;
                }
                if (swap_v) {
                    std::swap(neighbor.v_sequence[p], neighbor.v_sequence[q]);
                }
                if (swap_h) {
                    std::swap(neighbor.h_sequence[p], neighbor.h_sequence[q]);
                }
            }
        }
    }
}

}  // namespace

bool SA_solver_t::sample_p(double delta_c) const {
    return random_helper::sample(std::exp(-delta_c / t));
}

double SA_solver_t::get_delta(const sequence_pair_t& ori,
                              const sequence_pair_t& after) {
    const double ori_wirelength = ori.lp_predicted_wirelength;
    const double after_wirelength = after.lp_predicted_wirelength;
    if (after_wirelength <= 0 || ori_wirelength <= 0) {
        return 0;
    }
    return (after_wirelength - ori_wirelength) / ori_wirelength * 10.0;
}

void SA_solver_t::run(sequence_pair_enumerator_t& SPEN, double timeout,
                      double init_t, double end_t, bool load_back,
                      bool overlap) {
    parameters_init(init_t, end_t);
    runtime_timer.timer_start();
    time_limit = timeout;

    SPEN.updated_best_SP();
    sequence_pair_t best_sp = SPEN.best_SP;
    double best_wirelength = best_sp.get_wirelength();
    for (int it = 1;; ++it) {
        if (get_time_left() <= 0) {
            break;
        }
        it_timer.timer_start();

        sequence_pair_t& SP = SPEN.valid_sequence_pairs[0];
        sequence_pair_t after = find_neighbor_parallel(SP, overlap);
        const double SP_wirelength = SP.lp_predicted_wirelength;
        if (after.lp_predicted_wirelength != -1) {
            SP = after;
        }
        if (SP_wirelength < best_wirelength && SP_wirelength != -1) {
            best_sp = SP;
            best_wirelength = SP_wirelength;
        } else if (load_back && SP_wirelength > 1.375 * best_wirelength) {
            cout << "Load back..." << endl;
            SP = best_sp;  // avoid meaningless searching
        }

        runtime_timer.timer_end();
        runtime_timer.print_time_elapsed();
        cout << "It : " << it << ", t = " << t << endl;
        it_timer.timer_end();
        it_timer.print_time_elapsed();
        best_sp.print_inline();
        cout << "current best wirelength : " << std::setprecision(16)
             << best_wirelength << endl;
        cout << "current wirelength : " << std::setprecision(16)
             << SP_wirelength << endl;
        cout << "------------------------------" << endl;

        if (get_time_left() <= 0) {
            break;
        }
        t *= r;
        it_timer.timer_end();
        it_time = it_timer.get_time_elapsed();
        update_parameters();
    }
    SPEN.valid_sequence_pairs[0] = best_sp;  // keep the best solution
    best_sp.sequence_pair_validation();
}

sequence_pair_t SA_solver_t::find_neighbor_parallel(const sequence_pair_t& SP,
                                                    bool overlap) {
    const int n = sequence_pair_t::sequence_n;
    vector<int> v_map(n), h_map(n);
    for (int i = 0; i < n; ++i) {
        v_map[SP.v_sequence[i]] = h_map[SP.h_sequence[i]] = i;
    }
    const vector<int> rand_i = random_helper::rand_list(n);
    const vector<int> rand_j = random_helper::rand_list(n);

    // More threads once the temperature is low (fewer moves are accepted).
    // There must not be more threads than modules.
    const int thread_n = std::min(t >= 0.1 ? 3 : 6, n);
    const double chunk = static_cast<double>(n) / thread_n;

    neighbor_search_t search;
    vector<std::thread> threads;
    for (int i = 0; i < thread_n; ++i) {
        threads.emplace_back(search_neighbors, static_cast<int>(i * chunk),
                             static_cast<int>((i + 1) * chunk), std::cref(rand_i),
                             std::cref(rand_j), std::cref(v_map), std::cref(h_map),
                             std::cref(*this), std::cref(SP), overlap,
                             std::ref(search));
    }
    for (auto& thread : threads) {
        thread.join();
    }
    return search.result ? *search.result : SP;
}

void SA_solver_t::parameters_init(double init_t, double end_t) {
    this->t = init_t;
    this->r = 0.999;
    this->end_t = end_t;
}

double SA_solver_t::get_time_left() {
    runtime_timer.timer_end();
    return std::max(time_limit - runtime_timer.get_time_elapsed(), 0.0);
}

void SA_solver_t::update_parameters() {
    // Choose the cooling rate so that t reaches end_t when the time is up.
    if (t <= 0) {
        return;
    }
    const double it_left =
        std::max(get_time_left() / std::max(it_time, 1e-7), 1.0);
    r = std::pow(end_t / t, 1 / it_left);
}
