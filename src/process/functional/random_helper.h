//
// Created by RayChang on 2023/7/14.
//

#ifndef ICCAD2023PD_RANDOM_HELPER_H
#define ICCAD2023PD_RANDOM_HELPER_H

#include <cstdint>
#include <vector>

// Thread-safe pseudo random number helpers.
//
// Every thread owns its own engine, so the helpers may be used concurrently
// (e.g. by the SA worker threads). The thread calling set_seed() is seeded with
// exactly `seed`; other threads derive distinct seeds from it.
class random_helper {
   public:
    static void set_seed();               // non-deterministic seed
    static void set_seed(uint32_t seed);  // reproducible seed
    static uint32_t get_seed();

    static int get_rand();         // uniform in [0, 2^31)
    static bool sample(double p);  // true with probability p

    static std::vector<int> rand_list(int n);  // random permutation of 0..n-1
    static std::vector<int> get_random_int_vec(int n);
    static void shuffle(std::vector<int>& a);
    static void shuffle(std::vector<std::vector<int>>& a);
};

#endif  // ICCAD2023PD_RANDOM_HELPER_H
