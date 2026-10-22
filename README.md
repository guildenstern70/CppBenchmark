# CppBenchmark

A fast, cross-platform CPU architecture benchmark written in C++20 for modern Apple Silicon (ARM64) and Intel/AMD (x86_64) processors.

Designed to complete in **~11-12 seconds** on any system by using time-bounded throughput measurement.

## Benchmarked Subsystems

1. **ALU Single-Core Compute** (2.0s): Stresses 64-bit integer ALU throughput, bitwise rotations, arithmetic, and instruction-level parallelism.
2. **Cache Hierarchy** (2.0s): Measures L1/L2 cache read/write bandwidth with a 64 KB in-cache buffer.
3. **Memory Bandwidth** (2.0s): Evaluates DRAM streaming bandwidth using a triad kernel (`C[i] = A[i] + k * B[i]`) across a 256 MB buffer exceeding CPU caches.
4. **Memory Latency** (2.0s): Measures true random DRAM access latency (in nanoseconds) using pointer-chasing across a 128 MB random cyclic buffer, defeating hardware prefetchers.
5. **Multi-Threaded ALU** (3.0s): Measures multi-core compute throughput and parallel speedup across all available hardware threads (`std::thread::hardware_concurrency()`).

## Scoring

- Individual scores are calculated from throughput metrics.
- The **Final Composite Score** uses a geometric mean: $\sqrt[5]{\prod_{i=1}^5 \text{Score}_i}$.

## How to Build & Run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/CppBenchmark
```
