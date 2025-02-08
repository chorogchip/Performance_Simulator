#include <iostream>
#include <random>

#include "../perf_sim/performance_simulator.h"


int main() {
    ps::CacheSimBasic cache_sim(16, 0, 5, 1);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(0, (size_t)((1<<16)-1));

    for (int epoch = 0; epoch < 100; ++epoch) {
        for (int i = 0; i < 32; ++i) {
            const size_t address = dis(gen);
            cache_sim.Address(address);
        }
        cache_sim.Plot();
    }
    std::cout << cache_sim.Print() << '\n';
}