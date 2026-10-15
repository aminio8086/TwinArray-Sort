/*
 * TwinArray Sort tests
 * Copyright 2026 Amin Amini
 * SPDX-License-Identifier: Apache-2.0
 */

#include "twinarray_sort.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

static_assert(twinarray::TwinArrayInteger<int>);
static_assert(twinarray::TwinArrayInteger<unsigned int>);
static_assert(twinarray::TwinArrayInteger<std::int8_t>);
static_assert(twinarray::TwinArrayInteger<std::uint8_t>);
static_assert(!twinarray::TwinArrayInteger<bool>);
static_assert(!twinarray::TwinArrayInteger<char>);
static_assert(!twinarray::TwinArrayInteger<wchar_t>);
static_assert(!twinarray::TwinArrayInteger<char8_t>);
static_assert(!twinarray::TwinArrayInteger<char16_t>);
static_assert(!twinarray::TwinArrayInteger<char32_t>);

namespace {

struct TestContext {
    std::size_t passed = 0;
    std::size_t failed = 0;

    void check(const bool condition, const std::string& label, const int line)
    {
        if (condition) {
            ++passed;
        } else {
            ++failed;
            std::cerr << "[FAIL] line " << line << ": " << label << '\n';
        }
    }
};

template <twinarray::TwinArrayInteger T>
void verify_both_apis(
    TestContext& ctx,
    const std::vector<T>& input,
    const std::string& label,
    const int line)
{
    std::vector<T> expected = input;
    std::sort(expected.begin(), expected.end());

    const std::vector<T> original_snapshot = input;
    const std::vector<T> copy_sorted = twinarray::twinarray_sort_copy(input);

    ctx.check(input == original_snapshot, label + " (copy API preserves input)", line);
    ctx.check(copy_sorted == expected, label + " (copy API matches std::sort)", line);

    std::vector<T> direct_sorted = input;
    twinarray::twinarray_sort(direct_sorted);
    ctx.check(direct_sorted == expected, label + " (direct-output API matches std::sort)", line);
}

template <typename ExceptionType, twinarray::TwinArrayInteger T>
void verify_throws_both_apis(
    TestContext& ctx,
    const std::vector<T>& input,
    const std::string& label,
    const int line)
{
    bool copy_ok = false;
    try {
        (void)twinarray::twinarray_sort_copy(input);
    } catch (const ExceptionType&) {
        copy_ok = true;
    } catch (...) {
        copy_ok = false;
    }
    ctx.check(copy_ok, label + " (copy API exception)", line);

    bool direct_ok = false;
    try {
        std::vector<T> mutable_input = input;
        twinarray::twinarray_sort(mutable_input);
    } catch (const ExceptionType&) {
        direct_ok = true;
    } catch (...) {
        direct_ok = false;
    }
    ctx.check(direct_ok, label + " (direct-output API exception)", line);
}

void run_deterministic_tests(TestContext& ctx)
{
    verify_both_apis<int>(ctx, {}, "empty vector", __LINE__);
    verify_both_apis<int>(ctx, {0}, "single zero", __LINE__);
    verify_both_apis<int>(ctx, {42}, "single positive", __LINE__);
    verify_both_apis<int>(ctx, {0, 1}, "two sorted with zero", __LINE__);
    verify_both_apis<int>(ctx, {1, 0}, "two reversed with zero", __LINE__);
    verify_both_apis<int>(ctx, {5, 5}, "two identical", __LINE__);
    verify_both_apis<int>(ctx, {0, 0}, "two zeroes", __LINE__);
    verify_both_apis<int>(ctx, {0,1,2,3,4,5,6,7,8,9}, "already sorted", __LINE__);
    verify_both_apis<int>(ctx, {9,8,7,6,5,4,3,2,1,0}, "reverse sorted", __LINE__);
    verify_both_apis<int>(ctx, {9,3,7,1,0,8,2}, "unique including zero", __LINE__);
    verify_both_apis<int>(ctx, {9,3,7,1,8,2}, "unique excluding zero", __LINE__);
    verify_both_apis<int>(ctx, {9,3,7,1,3,0,8,2}, "duplicates with zero", __LINE__);
    verify_both_apis<int>(ctx, {5,2,9,2,5,1}, "duplicates without zero", __LINE__);
    verify_both_apis<int>(ctx, {5,0,2,0,9,0}, "duplicate zero", __LINE__);
    verify_both_apis<int>(ctx, {7,7,7,7,7,7}, "all equal positive", __LINE__);
    verify_both_apis<int>(ctx, {0,0,0,0,0}, "all zero", __LINE__);
    verify_both_apis<int>(ctx, {0,0,0,4,1,0,4,4,2,1,0,9}, "mixed multiplicities", __LINE__);
    verify_both_apis<int>(ctx, {100000,0,50000,1}, "sparse unique domain", __LINE__);
    verify_both_apis<int>(ctx, {100000,2,50000,2,0,0}, "sparse duplicate domain", __LINE__);
    verify_both_apis<std::int8_t>(ctx, {120,0,45,12,45,1}, "int8_t", __LINE__);
    verify_both_apis<std::uint8_t>(ctx, {250,0,250,128,0,1,255}, "uint8_t", __LINE__);
    verify_both_apis<std::int16_t>(ctx, {3000,0,1500,20,3000}, "int16_t", __LINE__);
    verify_both_apis<std::uint16_t>(ctx, {60000,0,1234,60000,42}, "uint16_t", __LINE__);
    verify_both_apis<std::int32_t>(ctx, {50000,0,10000,25000,10000}, "int32_t", __LINE__);
    verify_both_apis<std::uint32_t>(ctx, {50000U,0U,10000U,25000U}, "uint32_t", __LINE__);
    verify_both_apis<std::int64_t>(ctx, {40000LL,0LL,100LL,40000LL}, "int64_t", __LINE__);
    verify_both_apis<std::uint64_t>(ctx, {75000ULL,0ULL,500ULL,12500ULL}, "uint64_t", __LINE__);

    std::vector<std::uint8_t> high_multiplicity(600, 7);
    high_multiplicity[0] = 0;
    high_multiplicity[299] = 255;
    verify_both_apis<std::uint8_t>(ctx, high_multiplicity, "uint8_t multiplicity exceeds 255", __LINE__);

    verify_both_apis<std::size_t>(
        ctx,
        {std::numeric_limits<std::size_t>::max()},
        "maximum size_t singleton",
        __LINE__);
}

void run_exception_tests(TestContext& ctx)
{
    verify_throws_both_apis<std::invalid_argument, int>(ctx, {-1}, "negative singleton", __LINE__);
    verify_throws_both_apis<std::invalid_argument, int>(ctx, {1,-1}, "negative second value", __LINE__);
    verify_throws_both_apis<std::invalid_argument, int>(ctx, {-1,1}, "negative first value", __LINE__);
    verify_throws_both_apis<std::invalid_argument, int>(ctx, {0,-1,0}, "negative surrounded by zeroes", __LINE__);
    verify_throws_both_apis<std::invalid_argument, int>(
        ctx, {std::numeric_limits<int>::min()}, "INT_MIN singleton", __LINE__);

    const std::size_t max_sz = std::numeric_limits<std::size_t>::max();
    verify_throws_both_apis<std::length_error, std::size_t>(
        ctx, {0,max_sz}, "k == SIZE_MAX for non-singleton input", __LINE__);

    const std::size_t max_count_capacity = std::vector<std::size_t>{}.max_size();
    if (max_count_capacity < max_sz) {
        verify_throws_both_apis<std::length_error, std::size_t>(
            ctx, {0,max_count_capacity}, "k + 1 exceeds count_store max_size", __LINE__);
    }
}

void run_randomised_tests(TestContext& ctx)
{
    std::mt19937_64 rng(20260130ULL);

    std::uniform_int_distribution<std::size_t> small_n_dist(0, 25);
    for (int trial = 0; trial < 600; ++trial) {
        const std::size_t n = small_n_dist(rng);
        const int max_k = trial % 30;
        std::uniform_int_distribution<int> value_dist(0, max_k);
        std::vector<int> data(n);
        for (auto& value : data) value = value_dist(rng);
        verify_both_apis(ctx, data, "random small/dense", __LINE__);
    }

    for (int trial = 0; trial < 500; ++trial) {
        const std::size_t k = static_cast<std::size_t>(20 + (trial % 400));
        const std::size_t n = 1 + (static_cast<std::size_t>(trial) % k);

        std::vector<std::uint32_t> pool(k);
        std::iota(pool.begin(), pool.end(), 1U);
        std::shuffle(pool.begin(), pool.end(), rng);

        std::vector<std::uint32_t> data;
        data.reserve(n);
        data.push_back(0U);
        data.insert(data.end(), pool.begin(), pool.begin() + static_cast<std::ptrdiff_t>(n - 1));
        std::shuffle(data.begin(), data.end(), rng);
        verify_both_apis(ctx, data, "random unique with zero", __LINE__);
    }

    for (int trial = 0; trial < 500; ++trial) {
        const std::size_t k = static_cast<std::size_t>(20 + (trial % 400));
        const std::size_t n = 1 + (static_cast<std::size_t>(trial) % k);

        std::vector<std::uint32_t> pool(k);
        std::iota(pool.begin(), pool.end(), 1U);
        std::shuffle(pool.begin(), pool.end(), rng);

        std::vector<std::uint32_t> data(
            pool.begin(), pool.begin() + static_cast<std::ptrdiff_t>(n));
        verify_both_apis(ctx, data, "random unique without zero", __LINE__);
    }

    std::uniform_int_distribution<std::size_t> medium_n_dist(50, 500);
    std::uniform_int_distribution<std::uint64_t> dense_k_dist(1, 100);
    for (int trial = 0; trial < 500; ++trial) {
        const std::size_t n = medium_n_dist(rng);
        const std::uint64_t k = dense_k_dist(rng);
        std::uniform_int_distribution<std::uint64_t> value_dist(0, k);
        std::vector<std::uint64_t> data(n);
        for (auto& value : data) value = value_dist(rng);
        verify_both_apis(ctx, data, "random dense duplicates", __LINE__);
    }

    for (int trial = 0; trial < 300; ++trial) {
        const std::size_t n = 2 + static_cast<std::size_t>(trial % 150);
        std::uniform_int_distribution<int> value_dist(0, 500);
        std::vector<int> data(n, 0);
        for (auto& value : data) {
            if ((rng() & 3ULL) == 0ULL) value = value_dist(rng);
        }
        verify_both_apis(ctx, data, "random zero-heavy", __LINE__);
    }

    std::uniform_int_distribution<std::size_t> sparse_n_dist(2, 100);
    std::uniform_int_distribution<int> sparse_value_dist(0, 200000);
    for (int trial = 0; trial < 200; ++trial) {
        const std::size_t n = sparse_n_dist(rng);
        std::vector<int> data(n);
        for (auto& value : data) value = sparse_value_dist(rng);
        verify_both_apis(ctx, data, "random sparse-domain", __LINE__);
    }
}

void generate_exhaustive(
    TestContext& ctx,
    std::vector<int>& data,
    const std::size_t position,
    const int alphabet_size)
{
    if (position == data.size()) {
        verify_both_apis(ctx, data, "exhaustive small-domain", __LINE__);
        return;
    }

    for (int value = 0; value < alphabet_size; ++value) {
        data[position] = value;
        generate_exhaustive(ctx, data, position + 1, alphabet_size);
    }
}

void run_exhaustive_small_domain_tests(TestContext& ctx)
{
    constexpr int alphabet_size = 5;
    for (std::size_t length = 0; length <= 6; ++length) {
        std::vector<int> data(length, 0);
        generate_exhaustive(ctx, data, 0, alphabet_size);
    }
}

} // namespace

int main()
{
    TestContext ctx;
    run_deterministic_tests(ctx);
    run_exception_tests(ctx);
    run_randomised_tests(ctx);
    run_exhaustive_small_domain_tests(ctx);

    if (ctx.failed != 0) {
        std::cerr << "TwinArray Sort tests FAILED: "
                  << ctx.failed << " failed, "
                  << ctx.passed << " passed.\n";
        return EXIT_FAILURE;
    }

    std::cout << "TwinArray Sort tests PASSED ("
              << ctx.passed << " checks verified).\n";
    return EXIT_SUCCESS;
}
