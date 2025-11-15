#include <iostream>
#include <vector>
#include <numeric>
#include <thread>
#include <random>
#include <algorithm>
#include <functional>
#include <chrono>

// Structure that sums a block of data
template<typename Iterator, typename T>
struct accumulate_block {
    void operator()(Iterator first, Iterator last, T& result) {
        result = std::accumulate(first, last, result);
    }
};

// Parallel implementation of std::accumulate
template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    
    unsigned long const length = std::distance(first, last);
    
    if (!length)
        return init;

    unsigned long const min_per_thread = 20;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads = std::thread::hardware_concurrency();
    unsigned long const num_threads =
        std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    unsigned long const block_size = length / num_threads;

    std::vector<T> results(num_threads);
    std::vector<std::thread> threads(num_threads - 1);

    Iterator block_start = first;

    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);

        threads[i] = std::thread(
            accumulate_block<Iterator, T>(),
            block_start, block_end, std::ref(results[i]));

        block_start = block_end;
    }

    // The main thread sums the last block
    accumulate_block<Iterator, T>()(block_start, last, results[num_threads - 1]);

    for (auto& t : threads)
        t.join();

    return std::accumulate(results.begin(), results.end(), init);
}

int main() {
    
    const size_t N = 104002065;
    std::vector<int> data(N);

    // Random number generator with fixed seed
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(1, 100);
    std::generate(data.begin(), data.end(), [&]() { return dist(rng); });

    // ====== Measure parallel time ======
    auto t0 = std::chrono::high_resolution_clock::now();
    int result_parallel = parallel_accumulate(data.begin(), data.end(), 0);
    auto t1 = std::chrono::high_resolution_clock::now();
    auto dur_parallel = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);

    // ====== Measure sequential time ======
    t0 = std::chrono::high_resolution_clock::now();
    int result_seq = std::accumulate(data.begin(), data.end(), 0);
    t1 = std::chrono::high_resolution_clock::now();
    auto dur_seq = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);

    // ====== Print results ======
    std::cout << "Parallel result:   " << result_parallel << "\n";
    std::cout << "Parallel time:     " << dur_parallel.count() << " microseconds\n";

    std::cout << "Sequential result: " << result_seq << "\n";
    std::cout << "Sequential time:   " << dur_seq.count() << " microseconds\n";

    return 0;
}
