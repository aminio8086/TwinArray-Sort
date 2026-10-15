# TwinArray Sort

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![License: Apache-2.0](https://img.shields.io/badge/License-Apache--2.0-green.svg)](LICENSE)
[![Paper DOI](https://img.shields.io/badge/DOI-10.3390%2Felectronics15030609-blue.svg)](https://doi.org/10.3390/electronics15030609)

Reference C++20 implementation of **TwinArray Sort**, the conditional
non-comparison integer sorting algorithm introduced in:

> Amin Amini, “TwinArray Sort: An Ultrarapid Conditional Non-Comparison Integer
> Sorting Algorithm”, *Electronics*, 15(3), 609, 2026.  
> https://doi.org/10.3390/electronics15030609

## Overview

TwinArray Sort operates on non-negative integer data. Given an input of `n`
elements with maximum key `k`, it uses two indexed auxiliary stores of length
`k + 1`:

- `value_store`, which records encountered key values at their matching index;
- `count_store`, which records the occurrence frequency of each key.

During the mapping pass, `has_duplicates` records whether any key occurs more
than once. Reconstruction then follows one of two paths:

1. **Unique data:** scan the populated `value_store` entries in key order.
2. **Duplicate-containing data:** reproduce each present key according to
   `count_store`.

This repository corresponds specifically to the original 2026 *Electronics*
publication. Algorithmic extensions beyond that publication are outside the
scope of this reference repository.

## Scope

- **Language:** C++20
- **Input values:** non-negative integers
- **Negative signed values:** rejected with `std::invalid_argument`
- **Library form:** header-only
- **Reference header:** `include/twinarray_sort.hpp`

Character and Boolean element types are intentionally excluded from the public
template concept; ordinary signed and unsigned integer types are supported.

## Requirements

Use a C++20 compiler and standard library that provide the C++20 safe integer
comparison utilities, including `std::in_range`.

The feature-test macro is:

```text
__cpp_lib_integer_comparison_functions >= 202002L
```

CMake 3.20 or newer is used for the supplied build configuration.

## Quick Start

```cpp
#include "twinarray_sort.hpp"

#include <iostream>
#include <vector>

int main()
{
    std::vector<int> values{9, 3, 7, 1, 3, 0, 8, 2};

    twinarray::twinarray_sort(values);

    for (const int value : values) {
        std::cout << value << ' ';
    }
}
```

Output:

```text
0 1 2 3 3 7 8 9
```

## API

### `twinarray::twinarray_sort`

```cpp
twinarray::twinarray_sort(values);
```

Sorts the supplied vector and reconstructs the sorted sequence directly into
the same vector.

This removes the need for a separate output vector but **does not make
TwinArray Sort an O(1)-auxiliary-space in-place algorithm**. The two `k + 1`
auxiliary stores remain required.

### `twinarray::twinarray_sort_copy`

```cpp
auto sorted = twinarray::twinarray_sort_copy(values);
```

Returns a sorted vector and leaves the original input unchanged.

## Invalid Negative Input

The original published algorithm has a non-negative integer input domain.

```cpp
std::vector<int> values{4, -2, 7};
twinarray::twinarray_sort(values); // throws std::invalid_argument
```

A negative singleton such as `{-1}` is also rejected.

## Complexity

For a non-singleton input, let:

- `n` = number of input elements;
- `k = max(A)`.

| API | Time | Auxiliary working storage | Separate output |
|---|---:|---:|---:|
| `twinarray_sort` | `O(n + k)` | `O(k)` | none |
| `twinarray_sort_copy` | `O(n + k)` | `O(k)` | `O(n)` |

The original paper accounts for the separate sorted output and therefore
describes total space as `O(n + k)`.

Because the indexed stores scale with `k`, very sparse key domains with an
extreme maximum value can require substantial memory.

## C++ Zero-Handling Note

`value_store` is zero-initialised, so `value_store[0]` alone cannot distinguish
an absent zero from a present zero.

The published algorithm checks `count_store[0]` and places zero at the front of
the unique-data result. This C++ implementation performs that equivalent check
before scanning the positive `value_store` entries. The resulting order is
identical while avoiding insertion at the front of a populated
`std::vector`.

See [`docs/algorithm.md`](docs/algorithm.md) for a fuller correspondence note.

## Building

Configure and build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Run the tests:

```bash
ctest --test-dir build -C Release --output-on-failure
```

Build the benchmark:

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DTWINARRAY_BUILD_BENCHMARKS=ON

cmake --build build --config Release
```

## Direct Compilation

Example with GCC:

```bash
g++ -std=c++20 -O3 -Iinclude \
  examples/example.cpp -o twinarray_example
```

For local benchmarking, GCC and Clang users may optionally use
`-march=native`. It enables architecture-specific optimisation opportunities;
it does not guarantee vectorisation of any particular loop.

## Testing

The repository includes:

- deterministic edge cases;
- signed and unsigned integer-width coverage;
- explicit negative-input rejection tests;
- domain-overflow and vector-capacity guards;
- randomised comparisons with `std::sort`;
- exhaustive enumeration of small domains;
- CI configured for GCC, Clang, Apple Clang and MSVC;
- an additional Linux ASan + UBSan CI job.

## Benchmarking

`benchmarks/benchmark.cpp` compares the C++ implementation with `std::sort`.

This is an **implementation benchmark**, not automatically a reproduction of
the full experimental study reported in the paper. Results depend on the input
domain, machine, compiler, standard library and build flags.

The driver:

- reports actual `k = max(A)`;
- guarantees unique/duplicate dataset invariants;
- performs warm-ups;
- alternates algorithm execution order;
- reports median timings;
- verifies every timed result.

See [`docs/benchmarking.md`](docs/benchmarking.md).

## Repository Structure

```text
include/       Header-only implementation
examples/      Minimal usage example
tests/         Correctness and robustness tests
benchmarks/    Local C++ benchmark driver
docs/          Algorithm and benchmarking notes
.github/       Continuous-integration workflow
```

## Citation

If you use TwinArray Sort in scholarly work, please cite:

> Amin Amini, “TwinArray Sort: An Ultrarapid Conditional Non-Comparison Integer
> Sorting Algorithm”, *Electronics*, vol. 15, no. 3, article 609, 2026.  
> https://doi.org/10.3390/electronics15030609

BibTeX:

```bibtex
@article{amini2026twinarray,
  author  = {Amini, Amin},
  title   = {{TwinArray Sort}: An Ultrarapid Conditional
             Non-Comparison Integer Sorting Algorithm},
  journal = {Electronics},
  volume  = {15},
  number  = {3},
  pages   = {609},
  year    = {2026},
  doi     = {10.3390/electronics15030609},
  url     = {https://doi.org/10.3390/electronics15030609}
}
```

A machine-readable [`CITATION.cff`](CITATION.cff) is included for GitHub's
citation interface. The journal identifies the publication as article 609; the
CFF preferred citation retains the DOI, journal, volume and issue without
misusing the CFF `start` field, which denotes a start page. The journal identifies the publication as article 609; the
CFF preferred citation retains the DOI, journal, volume and issue without
misusing the CFF `start` field, which denotes a start page.

## Licence

The software is distributed under the Apache License, Version 2.0. See
[`LICENSE`](LICENSE) and [`NOTICE`](NOTICE).

Software licensing and scholarly citation are separate: Apache-2.0 governs the
software, while academic users are requested to cite the original publication.

## Author

**Amin Amini**

Original publication:  
https://doi.org/10.3390/electronics15030609
