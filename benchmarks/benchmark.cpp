/*
 * TwinArray Sort benchmark
 * Copyright 2026 Amin Amini
 * SPDX-License-Identifier: Apache-2.0
 */

#include "twinarray_sort.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

std::uint64_t parse_nonnegative_u64(const char* text, const char* name)
{
    const std::string_view input{text};
    if (input.empty() || input.front() == '-') {
        throw std::invalid_argument(std::string(name) + " must be a non-negative integer.");
    }

    std::size_t parsed = 0;
    const std::uint64_t value = std::stoull(std::string(input), &parsed);
    if (parsed != input.size()) {
        throw std::invalid_argument(std::string(name) + " must be a non-negative integer.");
    }
    return value;
}

std::size_t checked_size_t(const std::uint64_t value, const char* name)
{
    if (!std::in_range<std::size_t>(value)) {
        throw std::invalid_argument(std::string(name) + " is too large.");
    }
    return static_cast<std::size_t>(value);
}

std::vector<std::uint64_t>
generate_unique_dataset(
    const std::size_t n,
    const std::uint64_t k,
    const std::uint64_t seed)
{
    if (n == 0) throw std::invalid_argument("n must be at least 1.");
    if (static_cast<std::uint64_t>(n - 1) > k) {
        throw std::invalid_argument("Unique dataset requires n <= k + 1.");
    }
    if (n == 1) return {k};

    const std::size_t pool_size = checked_size_t(k, "k");
    if (pool_size > std::vector<std::uint64_t>{}.max_size()) {
        throw std::length_error("k is too large for benchmark dataset generation.");
    }

    std::mt19937_64 rng(seed);
    std::vector<std::uint64_t> pool(pool_size);
    std::iota(pool.begin(), pool.end(), std::uint64_t{0});
    std::shuffle(pool.begin(), pool.end(), rng);

    std::vector<std::uint64_t> data;
    data.reserve(n);
    data.insert(data.end(), pool.begin(), pool.begin() + static_cast<std::ptrdiff_t>(n - 1));
    data.push_back(k);
    std::shuffle(data.begin(), data.end(), rng);
    return data;
}

std::vector<std::uint64_t>
generate_duplicate_dataset(
    const std::size_t n,
    const std::uint64_t k,
    const std::uint64_t seed)
{
    if (n < 2) {
        throw std::invalid_argument("Duplicate dataset requires n to be at least 2.");
    }

    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<std::uint64_t> dist(0, k);
    std::vector<std::uint64_t> data(n);
    for (auto& value : data) value = dist(rng);

    data[0] = k;
    if (n == 2) data[1] = k;
    else data[1] = data[2];

    std::shuffle(data.begin(), data.end(), rng);
    return data;
}

double median(std::vector<double> values)
{
    if (values.empty()) {
        throw std::invalid_argument("Cannot compute a median from zero runs.");
    }
    std::sort(values.begin(), values.end());
    const std::size_t middle = values.size() / 2;
    if ((values.size() % 2) != 0) return values[middle];
    return 0.5 * (values[middle - 1] + values[middle]);
}

template <typename SortFn>
double time_one(SortFn&& sort_fn)
{
    using clock_type = std::chrono::steady_clock;
    using ms_duration = std::chrono::duration<double, std::milli>;
    const auto start = clock_type::now();
    std::forward<SortFn>(sort_fn)();
    const auto finish = clock_type::now();
    return ms_duration(finish - start).count();
}

bool run_benchmark_section(
    const std::string& label,
    const std::vector<std::uint64_t>& base_data,
    const std::size_t runs,
    const std::size_t warmup_runs)
{
    if (runs == 0) throw std::invalid_argument("runs must be at least 1.");

    const std::uint64_t actual_k = *std::max_element(base_data.begin(), base_data.end());
    std::vector<std::uint64_t> expected = base_data;
    std::sort(expected.begin(), expected.end());

    std::vector<std::uint64_t> twin_work(base_data.size());
    std::vector<std::uint64_t> std_work(base_data.size());
    bool verified = true;

    for (std::size_t run = 0; run < warmup_runs; ++run) {
        twin_work = base_data;
        std_work = base_data;
        if ((run % 2) == 0) {
            twinarray::twinarray_sort(twin_work);
            std::sort(std_work.begin(), std_work.end());
        } else {
            std::sort(std_work.begin(), std_work.end());
            twinarray::twinarray_sort(twin_work);
        }
        verified = verified && twin_work == expected && std_work == expected;
    }

    std::vector<double> twin_times;
    std::vector<double> std_times;
    twin_times.reserve(runs);
    std_times.reserve(runs);

    for (std::size_t run = 0; run < runs; ++run) {
        twin_work = base_data;
        std_work = base_data;
        double twin_ms = 0.0;
        double std_ms = 0.0;

        if ((run % 2) == 0) {
            twin_ms = time_one([&] { twinarray::twinarray_sort(twin_work); });
            std_ms = time_one([&] { std::sort(std_work.begin(), std_work.end()); });
        } else {
            std_ms = time_one([&] { std::sort(std_work.begin(), std_work.end()); });
            twin_ms = time_one([&] { twinarray::twinarray_sort(twin_work); });
        }

        twin_times.push_back(twin_ms);
        std_times.push_back(std_ms);
        verified = verified && twin_work == expected && std_work == expected;
    }

    const double twin_median = median(twin_times);
    const double std_median = median(std_times);
    const double ratio = twin_median > 0.0 ? std_median / twin_median : 0.0;

    std::cout
        << "Dataset: " << label << '\n'
        << "n:       " << base_data.size() << '\n'
        << "k:       " << actual_k << '\n'
        << "Runs:    " << runs << "\n\n"
        << std::fixed << std::setprecision(2)
        << "TwinArray Sort median: " << twin_median << " ms\n"
        << "std::sort median:      " << std_median << " ms\n"
        << "Ratio (std/TwinArray): " << ratio << "x\n\n"
        << "Verification: " << (verified ? "PASS" : "FAIL")
        << "\n\n";

    return verified;
}

} // namespace

int main(int argc, char* argv[])
{
    try {
        std::size_t n = 1'000'000;
        std::uint64_t k = 2'000'000;
        std::size_t runs = 9;
        constexpr std::size_t warmup_runs = 2;
        constexpr std::uint64_t seed = 42;

        if (argc >= 2) n = checked_size_t(parse_nonnegative_u64(argv[1], "n"), "n");
        if (argc >= 3) k = parse_nonnegative_u64(argv[2], "k");
        if (argc >= 4) runs = checked_size_t(parse_nonnegative_u64(argv[3], "runs"), "runs");
        if (argc > 4) {
            throw std::invalid_argument("Usage: twinarray_benchmark [n] [k] [runs]");
        }
        if (runs == 0) throw std::invalid_argument("runs must be at least 1.");

        std::cout << "TwinArray Sort C++20 Benchmark\n"
                  << "Seed:    " << seed << "\n\n";

        const auto unique_data = generate_unique_dataset(n, k, seed);
        const auto duplicate_data = generate_duplicate_dataset(n, k, seed + 1);

        const bool unique_ok = run_benchmark_section("Unique", unique_data, runs, warmup_runs);
        const bool duplicate_ok = run_benchmark_section("Duplicate", duplicate_data, runs, warmup_runs);

        return (unique_ok && duplicate_ok) ? EXIT_SUCCESS : EXIT_FAILURE;
    } catch (const std::exception& ex) {
        std::cerr << "Benchmark error: " << ex.what() << '\n';
        return EXIT_FAILURE;
    }
}
