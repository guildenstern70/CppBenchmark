# C++ Modern CPU Architecture Benchmark         

## Apple M4 Pro

- CPU Model       : Apple M4 Pro
- Architecture    : ARM64
- Hardware Threads: 12 logical cores
- Target Duration : ~11 - 12 seconds total



## BENCHMARK RESULTS                             


1. ALU (Single-Core)        5.29 GigaOps/s                        635
2. Cache (L1/L2 Buffer)     64.00 GB/s                            960
3. Memory Bandwidth (DRAM)  118.57 GB/s                          2964
4. Memory Latency (Random)  106.4 ns                              940
5. Multi-Threaded ALU       49.3 GOPS (9.3x on 12c)              5922

- FINAL COMPOSITE SCORE (Geometric Mean):                          1587

## Apple M5 Max

=========================================================
CPU Model       : Apple M5 Max
Architecture    : ARM64
Hardware Threads: 18 logical cores
Target Duration : ~11 - 12 seconds total
=========================================================

=========================================================================
BENCHMARK RESULTS                             
=========================================================================
Benchmark Category          Raw Metric                   Score Points
-------------------------------------------------------------------------
1. ALU (Single-Core)        5.42 GigaOps/s                        650
2. Cache (L1/L2 Buffer)     65.52 GB/s                            983
3. Memory Bandwidth (DRAM)  120.63 GB/s                          3016
4. Memory Latency (Random)  97.3 ns                              1028
5. Multi-Threaded ALU       89.8 GOPS (16.6x on 18c)            10780
=========================================================================
FINAL COMPOSITE SCORE (Geometric Mean):                          1845 
=========================================================================
