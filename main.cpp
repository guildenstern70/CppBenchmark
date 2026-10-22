/*!
 * CPP BENCHMARK
 * A CPU C++ Benchmark for modern CPUs
 * (C) 2026 Alessio Saltarin
 * ISC License
 *
 !*/


#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <numeric>
#include <random>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <string>
#include <atomic>

#if defined(__APPLE__)
#include <sys/sysctl.h>
#endif

// Compiler barrier to prevent dead code elimination
template <typename T>
inline void do_not_optimize(T const& value) {
#if defined(__GNUC__) || defined(__clang__)
    asm volatile("" : : "g"(value) : "memory");
#else
    volatile T dummy = value;
    (void)dummy;
#endif
}

// -------------------------------------------------------------
// System Information Detection
// -------------------------------------------------------------
std::string get_cpu_brand() {
#if defined(__APPLE__)
    char buffer[256];
    size_t size = sizeof(buffer);
    if (sysctlbyname("machdep.cpu.brand_string", &buffer, &size, nullptr, 0) == 0) {
        return std::string(buffer);
    }
#endif
#if defined(__aarch64__) || defined(_M_ARM64)
    return "ARM64 Processor (Apple Silicon / ARM)";
#elif defined(__x86_64__) || defined(_M_X64)
    return "x86_64 Processor (Intel / AMD)";
#else
    return "Generic Processor";
#endif
}

std::string get_architecture() {
#if defined(__aarch64__) || defined(_M_ARM64)
    return "ARM64";
#elif defined(__x86_64__) || defined(_M_X64)
    return "x86_64";
#else
    return "Unknown Architecture";
#endif
}

// -------------------------------------------------------------
// Test 1: ALU Single-Core Compute (Integer & Arithmetic Throughput)
// -------------------------------------------------------------
struct AluResult {
    double giga_ops_per_sec;
    double score;
};

AluResult benchmark_alu(double target_seconds = 2.0) {
    using clock = std::chrono::steady_clock;
    uint64_t a = 0x9e3779b97f4a7c15ULL;
    uint64_t b = 0xbf58476d1ce4e5b9ULL;
    uint64_t c = 0x94d049bb133111ebULL;
    uint64_t d = 0x2545f4914f6cdd1dULL;

    const uint64_t inner_loops = 100000;
    // 32 operations per unrolled step
    const uint64_t ops_per_step = 32;

    uint64_t total_operations = 0;
    auto start_time = clock::now();
    auto end_time = start_time;

    while (true) {
        for (uint64_t i = 0; i < inner_loops; ++i) {
            a += b * 31; b ^= (a >> 7) | (a << 57);
            c += d * 37; d ^= (c >> 11) | (c << 53);
            a ^= c + 0x517cc1b727220a95ULL;
            b += d ^ 0x9e3779b97f4a7c15ULL;
            c ^= (b << 13) | (b >> 51);
            d += (a * 41) ^ 0xbf58476d1ce4e5b9ULL;

            a += (d ^ 0x6a09e667f3bcc908ULL) * 13;
            b ^= (a >> 17) | (a << 47);
            c += (b ^ 0xbb67ae8584caa73bULL) * 19;
            d ^= (c >> 23) | (c << 41);

            a ^= b + 0x3c6ef372fe94f82bULL;
            b += c ^ 0xa54ff53a5f1d36f1ULL;
            c ^= d + 0x510e527fade682d1ULL;
            d += a ^ 0x9b05688c2b3e6c1fULL;
        }
        total_operations += inner_loops * ops_per_step;
        end_time = clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;
        if (elapsed.count() >= target_seconds) break;
    }

    do_not_optimize(a + b + c + d);

    std::chrono::duration<double> elapsed = end_time - start_time;
    double gops = (static_cast<double>(total_operations) / elapsed.count()) / 1e9;
    double score = gops * 120.0;
    return {gops, score};
}

// -------------------------------------------------------------
// Test 2: Cache Hierarchy Throughput (L1/L2 Cache Speed)
// -------------------------------------------------------------
struct CacheResult {
    double gb_per_sec;
    double score;
};

CacheResult benchmark_cache(double target_seconds = 2.0) {
    using clock = std::chrono::steady_clock;
    // 64 KB buffer (fits into L1/L2 data cache)
    const size_t buffer_bytes = 64 * 1024;
    const size_t element_count = buffer_bytes / sizeof(uint64_t);
    std::vector<uint64_t> buffer(element_count, 0x123456789ABCDEF0ULL);

    uint64_t passes = 0;
    uint64_t accumulator = 0;
    auto start_time = clock::now();
    auto end_time = start_time;

    while (true) {
        for (size_t i = 0; i < element_count; i += 4) {
            accumulator += buffer[i];
            buffer[i] = accumulator ^ 0x5555555555555555ULL;

            accumulator += buffer[i + 1];
            buffer[i + 1] = accumulator ^ 0xAAAAAAAAAAAAAAAAULL;

            accumulator += buffer[i + 2];
            buffer[i + 2] = accumulator ^ 0x3333333333333333ULL;

            accumulator += buffer[i + 3];
            buffer[i + 3] = accumulator ^ 0xCCCCCCCCCCCCCCCCULL;
        }
        passes++;
        if ((passes & 0x3FFF) == 0) {
            end_time = clock::now();
            std::chrono::duration<double> elapsed = end_time - start_time;
            if (elapsed.count() >= target_seconds) break;
        }
    }

    do_not_optimize(accumulator);
    do_not_optimize(buffer.data());

    std::chrono::duration<double> elapsed = end_time - start_time;
    // Each element is read and written (2 memory operations * 8 bytes = 16 bytes per element)
    double total_bytes = static_cast<double>(passes) * element_count * sizeof(uint64_t) * 2.0;
    double gb_per_sec = (total_bytes / elapsed.count()) / (1024.0 * 1024.0 * 1024.0);
    double score = gb_per_sec * 15.0;
    return {gb_per_sec, score};
}

// -------------------------------------------------------------
// Test 3: Memory Bandwidth (DRAM Streaming Triad)
// -------------------------------------------------------------
struct MemoryBandwidthResult {
    double gb_per_sec;
    double score;
};

MemoryBandwidthResult benchmark_memory_bandwidth(double target_seconds = 2.0) {
    using clock = std::chrono::steady_clock;
    // 256 MB total working set (exceeds CPU L3/SLC cache)
    const size_t array_elements = (256 * 1024 * 1024) / (sizeof(double) * 3);
    std::vector<double> a(array_elements, 1.0);
    std::vector<double> b(array_elements, 2.0);
    std::vector<double> c(array_elements, 0.0);

    const double scalar = 3.141592653589793;
    uint64_t passes = 0;
    auto start_time = clock::now();
    auto end_time = start_time;

    while (true) {
        for (size_t i = 0; i < array_elements; ++i) {
            c[i] = a[i] + scalar * b[i];
        }
        passes++;
        do_not_optimize(c.data());
        end_time = clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;
        if (elapsed.count() >= target_seconds) break;
    }

    std::chrono::duration<double> elapsed = end_time - start_time;
    // STREAM Triad transfers 3 arrays: 2 reads (a, b) and 1 write (c) = 3 * sizeof(double)
    double total_bytes = static_cast<double>(passes) * array_elements * sizeof(double) * 3.0;
    double gb_per_sec = (total_bytes / elapsed.count()) / (1024.0 * 1024.0 * 1024.0);
    double score = gb_per_sec * 25.0;
    return {gb_per_sec, score};
}

// -------------------------------------------------------------
// Test 4: Memory Latency (Random Pointer Chasing)
// -------------------------------------------------------------
struct MemoryLatencyResult {
    double latency_ns;
    double score;
};

MemoryLatencyResult benchmark_memory_latency(double target_seconds = 2.0) {
    using clock = std::chrono::steady_clock;
    // 128 MB buffer of cache lines (64 bytes each)
    const size_t line_size = 64;
    const size_t buffer_bytes = 128 * 1024 * 1024;
    const size_t num_lines = buffer_bytes / line_size;

    struct alignas(64) Node {
        uint32_t next;
        uint8_t padding[60];
    };

    std::vector<Node> nodes(num_lines);
    std::vector<uint32_t> permutation(num_lines);
    std::iota(permutation.begin(), permutation.end(), 0);

    // Generate a single random cycle traversing every cache line
    std::mt19937_64 rng(1337);
    for (size_t i = num_lines - 1; i > 0; --i) {
        std::uniform_int_distribution<size_t> dist(0, i - 1);
        size_t j = dist(rng);
        std::swap(permutation[i], permutation[j]);
    }
    for (size_t i = 0; i < num_lines - 1; ++i) {
        nodes[permutation[i]].next = permutation[i + 1];
    }
    nodes[permutation[num_lines - 1]].next = permutation[0];

    uint32_t current = 0;
    uint64_t total_hops = 0;
    const uint64_t hop_batch = 200000;
    auto start_time = clock::now();
    auto end_time = start_time;

    while (true) {
        for (uint64_t i = 0; i < hop_batch; ++i) {
            current = nodes[current].next;
        }
        total_hops += hop_batch;
        end_time = clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;
        if (elapsed.count() >= target_seconds) break;
    }

    do_not_optimize(current);

    std::chrono::duration<double> elapsed = end_time - start_time;
    double latency_ns = (elapsed.count() * 1e9) / static_cast<double>(total_hops);
    double score = (latency_ns > 0.0) ? (100000.0 / latency_ns) : 0.0;
    return {latency_ns, score};
}

// -------------------------------------------------------------
// Test 5: Multi-Core Threading (Parallel Scalability)
// -------------------------------------------------------------
struct ThreadingResult {
    double total_giga_ops;
    double speedup;
    double score;
    unsigned int thread_count;
};

ThreadingResult benchmark_threading(double single_core_gops, double target_seconds = 3.0) {
    using clock = std::chrono::steady_clock;
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;

    std::vector<std::thread> workers;
    std::atomic<bool> stop_flag{false};
    std::vector<uint64_t> thread_ops(num_threads, 0);

    auto worker_task = [&](unsigned int tid) {
        uint64_t a = 0x9e3779b97f4a7c15ULL + tid;
        uint64_t b = 0xbf58476d1ce4e5b9ULL ^ (tid * 0x100000001B3ULL);
        uint64_t c = 0x94d049bb133111ebULL + (tid * 17);
        uint64_t d = 0x2545f4914f6cdd1dULL ^ (tid * 31);

        const uint64_t inner_loops = 50000;
        const uint64_t ops_per_step = 32;
        uint64_t local_ops = 0;

        while (!stop_flag.load(std::memory_order_relaxed)) {
            for (uint64_t i = 0; i < inner_loops; ++i) {
                a += b * 31; b ^= (a >> 7) | (a << 57);
                c += d * 37; d ^= (c >> 11) | (c << 53);
                a ^= c + 0x517cc1b727220a95ULL;
                b += d ^ 0x9e3779b97f4a7c15ULL;
                c ^= (b << 13) | (b >> 51);
                d += (a * 41) ^ 0xbf58476d1ce4e5b9ULL;

                a += (d ^ 0x6a09e667f3bcc908ULL) * 13;
                b ^= (a >> 17) | (a << 47);
                c += (b ^ 0xbb67ae8584caa73bULL) * 19;
                d ^= (c >> 23) | (c << 41);

                a ^= b + 0x3c6ef372fe94f82bULL;
                b += c ^ 0xa54ff53a5f1d36f1ULL;
                c ^= d + 0x510e527fade682d1ULL;
                d += a ^ 0x9b05688c2b3e6c1fULL;
            }
            local_ops += inner_loops * ops_per_step;
        }

        do_not_optimize(a + b + c + d);
        thread_ops[tid] = local_ops;
    };

    auto start_time = clock::now();
    for (unsigned int t = 0; t < num_threads; ++t) {
        workers.emplace_back(worker_task, t);
    }

    std::this_thread::sleep_for(std::chrono::duration<double>(target_seconds));
    stop_flag.store(true, std::memory_order_relaxed);

    for (auto& w : workers) {
        if (w.joinable()) w.join();
    }
    auto end_time = clock::now();

    std::chrono::duration<double> elapsed = end_time - start_time;
    uint64_t total_ops = std::accumulate(thread_ops.begin(), thread_ops.end(), 0ULL);
    double total_gops = (static_cast<double>(total_ops) / elapsed.count()) / 1e9;
    double speedup = (single_core_gops > 0.0) ? (total_gops / single_core_gops) : 1.0;
    double score = total_gops * 120.0;

    return {total_gops, speedup, score, num_threads};
}

// -------------------------------------------------------------
// Main Benchmark Runner & Reporting
// -------------------------------------------------------------
int main() {
    std::cout << "=========================================================\n";
    std::cout << "           C++ Modern CPU Architecture Benchmark         \n";
    std::cout << "=========================================================\n";
    std::cout << " CPU Model       : " << get_cpu_brand() << "\n";
    std::cout << " Architecture    : " << get_architecture() << "\n";
    std::cout << " Hardware Threads: " << std::thread::hardware_concurrency() << " logical cores\n";
    std::cout << " Target Duration : ~11 - 12 seconds total\n";
    std::cout << "=========================================================\n\n";

    std::cout << "[1/5] Running ALU Single-Core Compute (2.0s)... " << std::flush;
    auto alu = benchmark_alu(2.0);
    std::cout << "DONE (" << std::fixed << std::setprecision(2) << alu.giga_ops_per_sec << " GigaOps/s)\n";

    std::cout << "[2/5] Running Cache Hierarchy Throughput (2.0s)... " << std::flush;
    auto cache = benchmark_cache(2.0);
    std::cout << "DONE (" << std::fixed << std::setprecision(2) << cache.gb_per_sec << " GB/s)\n";

    std::cout << "[3/5] Running Memory Bandwidth [DRAM Streaming] (2.0s)... " << std::flush;
    auto mem_bw = benchmark_memory_bandwidth(2.0);
    std::cout << "DONE (" << std::fixed << std::setprecision(2) << mem_bw.gb_per_sec << " GB/s)\n";

    std::cout << "[4/5] Running Memory Latency [Pointer Chasing] (2.0s)... " << std::flush;
    auto mem_lat = benchmark_memory_latency(2.0);
    std::cout << "DONE (" << std::fixed << std::setprecision(2) << mem_lat.latency_ns << " ns)\n";

    std::cout << "[5/5] Running Multi-Core Threading Scalability (3.0s)... " << std::flush;
    auto threading = benchmark_threading(alu.giga_ops_per_sec, 3.0);
    std::cout << "DONE (" << std::fixed << std::setprecision(2) << threading.total_giga_ops 
              << " GigaOps/s, " << threading.speedup << "x speedup)\n\n";

    // Geometric mean composite score
    double composite_score = std::pow(
        alu.score * cache.score * mem_bw.score * mem_lat.score * threading.score,
        1.0 / 5.0
    );

    std::cout << "=========================================================================\n";
    std::cout << "                           BENCHMARK RESULTS                             \n";
    std::cout << "=========================================================================\n";
    std::cout << std::left 
              << std::setw(28) << "Benchmark Category" 
              << std::setw(26) << "Raw Metric" 
              << std::right << std::setw(15) << "Score Points" << "\n";
    std::cout << "-------------------------------------------------------------------------\n";

    auto format_val = [](double val, int precision, const std::string& unit) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << val << " " << unit;
        return oss.str();
    };

    std::ostringstream multi_desc;
    multi_desc << std::fixed << std::setprecision(1) << threading.total_giga_ops << " GOPS ("
               << std::setprecision(1) << threading.speedup << "x on " << threading.thread_count << "c)";

    std::cout << std::left << std::setw(28) << "1. ALU (Single-Core)"
              << std::setw(26) << format_val(alu.giga_ops_per_sec, 2, "GigaOps/s")
              << std::right << std::setw(15) << std::fixed << std::setprecision(0) << alu.score << "\n";

    std::cout << std::left << std::setw(28) << "2. Cache (L1/L2 Buffer)"
              << std::setw(26) << format_val(cache.gb_per_sec, 2, "GB/s")
              << std::right << std::setw(15) << std::fixed << std::setprecision(0) << cache.score << "\n";

    std::cout << std::left << std::setw(28) << "3. Memory Bandwidth (DRAM)"
              << std::setw(26) << format_val(mem_bw.gb_per_sec, 2, "GB/s")
              << std::right << std::setw(15) << std::fixed << std::setprecision(0) << mem_bw.score << "\n";

    std::cout << std::left << std::setw(28) << "4. Memory Latency (Random)"
              << std::setw(26) << format_val(mem_lat.latency_ns, 1, "ns")
              << std::right << std::setw(15) << std::fixed << std::setprecision(0) << mem_lat.score << "\n";

    std::cout << std::left << std::setw(28) << "5. Multi-Threaded ALU"
              << std::setw(26) << multi_desc.str()
              << std::right << std::setw(15) << std::fixed << std::setprecision(0) << threading.score << "\n";

    std::cout << "=========================================================================\n";
    std::cout << std::left << std::setw(54) << "FINAL COMPOSITE SCORE (Geometric Mean):"
              << std::right << std::setw(15) << std::fixed << std::setprecision(0) << composite_score << "\n";
    std::cout << "=========================================================================\n";

    return 0;
}
