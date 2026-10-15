# Benchmarking Guide

`benchmarks/benchmark.cpp` compares this C++20 implementation of TwinArray Sort
with `std::sort`.

It is an implementation benchmark, not automatically a reproduction of the
complete experimental study reported in the 2026 *Electronics* paper.

## Interpretation

TwinArray Sort is a non-comparison integer sorting algorithm whose cost depends
on both `n` and the key-domain maximum `k`.

`std::sort` is the standard comparison-based sorting algorithm and has
`O(n log n)` comparison complexity. Standard-library implementations may use
different internal strategies, so this repository does not assume or require a
particular implementation such as introsort.

Relative performance can depend strongly on:

- `n`;
- `k`;
- key density;
- duplicate frequency;
- integer width;
- CPU cache hierarchy;
- memory bandwidth;
- allocator and operating-system behaviour;
- compiler and standard library;
- optimisation flags.

Do not generalise a speed ratio from one dataset to unrelated domains.

## Methodology

The benchmark:

- uses `std::chrono::steady_clock`;
- uses fixed pseudo-random seeds;
- guarantees that the Unique dataset contains no duplicates and contains `k`;
- guarantees that the Duplicate dataset contains a duplicate and contains `k`;
- copies working input buffers outside the timed regions;
- includes TwinArray Sort's internal auxiliary allocations in its timing;
- alternates which algorithm runs first on successive timed iterations;
- performs warm-up runs;
- reports medians across the requested measured runs;
- verifies both algorithms against an expected sorted vector after each run;
- refuses `runs = 0`.

Default parameters are:

```text
n = 1,000,000
k = 2,000,000
runs = 9
seed = 42
```

## Build

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DTWINARRAY_BUILD_BENCHMARKS=ON

cmake --build build --config Release
```

Run with defaults:

```bash
./build/twinarray_benchmark
```

Or specify:

```bash
./build/twinarray_benchmark [n] [k] [runs]
```

For example:

```bash
./build/twinarray_benchmark 500000 1000000 11
```

On Windows with a multi-config generator, the executable is normally under
`build/Release/`.

## Direct Compilation

GCC:

```bash
g++ -std=c++20 -O3 -march=native -Iinclude \
  benchmarks/benchmark.cpp -o twinarray_benchmark
```

Clang:

```bash
clang++ -std=c++20 -O3 -march=native -Iinclude \
  benchmarks/benchmark.cpp -o twinarray_benchmark
```

MSVC Developer Command Prompt:

```text
cl /std:c++20 /O2 /EHsc /W4 /I include \
  benchmarks\benchmark.cpp /Fe:twinarray_benchmark.exe
```

`-march=native` permits GCC/Clang to target instructions available on the local
processor. It does not guarantee vectorisation of any particular loop.

## Reporting Results

When publishing or discussing benchmark results, record at least:

- CPU model;
- RAM;
- operating system;
- compiler and version;
- standard library where relevant;
- exact compiler flags;
- `n`, `k`, seed and run count.
