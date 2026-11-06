//
// Created by RayChang on 2023/7/14.
//

#include "random_helper.h"

#include <atomic>
#include <chrono>
#include <random>
#include <utility>

namespace {

std::atomic<uint32_t> g_seed{5489u};
// Incremented by set_seed() so that every thread re-seeds lazily.
std::atomic<uint32_t> g_generation{1};
std::atomic<uint32_t> g_stream{0};

struct thread_engine_t {
    std::mt19937 engine;
    uint32_t generation = 0;  // 0: not seeded yet
};
thread_local thread_engine_t t_engine;

std::mt19937& engine() {
    const uint32_t generation = g_generation.load();
    if (t_engine.generation != generation) {
        std::seed_seq seq{g_seed.load(), g_stream.fetch_add(1) + 1};
        t_engine.engine.seed(seq);
        t_engine.generation = generation;
    }
    return t_engine.engine;
}

template <class T>
void shuffle_impl(std::vector<T>& a) {
    const int n = static_cast<int>(a.size());
    for (int i = 0; i < n; ++i) {
        std::swap(a[i], a[random_helper::get_rand() % n]);
    }
}

}  // namespace

void random_helper::set_seed() {
    std::random_device rd;
    const auto now = static_cast<uint32_t>(
        std::chrono::steady_clock::now().time_since_epoch().count());
    set_seed(rd() ^ now);
}

void random_helper::set_seed(uint32_t seed) {
    g_seed.store(seed);
    t_engine.engine.seed(seed);
    t_engine.generation = g_generation.fetch_add(1) + 1;
}

uint32_t random_helper::get_seed() { return g_seed.load(); }

int random_helper::get_rand() { return static_cast<int>(engine()() >> 1); }

bool random_helper::sample(double p) {
    constexpr int k = 100000000;
    return p * k >= get_rand() % k;
}

std::vector<int> random_helper::rand_list(int n) {
    std::vector<int> res(n);
    for (int i = 0; i < n; ++i) {
        res[i] = i;
    }
    shuffle_impl(res);
    return res;
}

std::vector<int> random_helper::get_random_int_vec(int n) {
    return rand_list(n);
}

void random_helper::shuffle(std::vector<int>& a) { shuffle_impl(a); }

void random_helper::shuffle(std::vector<std::vector<int>>& a) {
    shuffle_impl(a);
}
