/*
 * TwinArray Sort
 * Reference C++20 implementation of the algorithm introduced in:
 *
 * Amin Amini,
 * "TwinArray Sort: An Ultrarapid Conditional Non-Comparison
 * Integer Sorting Algorithm",
 * Electronics 15(3), 609, 2026.
 * https://doi.org/10.3390/electronics15030609
 *
 * Copyright 2026 Amin Amini
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TWINARRAY_SORT_HPP
#define TWINARRAY_SORT_HPP

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace twinarray {

/**
 * @brief Element types supported by this reference implementation.
 *
 * TwinArray Sort operates on integer keys. Character and Boolean types are
 * excluded so that the API expresses that intent clearly and remains
 * compatible with C++20 safe integer-comparison utilities.
 */
template <typename T>
concept TwinArrayInteger =
    std::integral<T> &&
    (!std::same_as<std::remove_cv_t<T>, bool>) &&
    (!std::same_as<std::remove_cv_t<T>, char>) &&
    (!std::same_as<std::remove_cv_t<T>, wchar_t>) &&
    (!std::same_as<std::remove_cv_t<T>, char8_t>) &&
    (!std::same_as<std::remove_cv_t<T>, char16_t>) &&
    (!std::same_as<std::remove_cv_t<T>, char32_t>);

namespace detail {

template <TwinArrayInteger T>
void validate_singleton(const T value)
{
    if constexpr (std::signed_integral<T>) {
        if (value < T{0}) {
            throw std::invalid_argument(
                "TwinArray Sort requires non-negative integers.");
        }
    }
}

/**
 * @brief Validates the non-negative input domain and returns k + 1.
 *
 * This helper is used for inputs with at least two elements. It verifies that
 * every key can be represented as std::size_t and that the indexed auxiliary
 * stores can represent the required domain before allocation is attempted.
 */
template <TwinArrayInteger T>
[[nodiscard]] std::size_t
checked_domain_size(const std::vector<T>& arr)
{
    T max_val = arr.front();

    if constexpr (std::signed_integral<T>) {
        if (max_val < T{0}) {
            throw std::invalid_argument(
                "TwinArray Sort requires non-negative integers.");
        }
    }

    for (std::size_t j = 1; j < arr.size(); ++j) {
        const T num = arr[j];

        if constexpr (std::signed_integral<T>) {
            if (num < T{0}) {
                throw std::invalid_argument(
                    "TwinArray Sort requires non-negative integers.");
            }
        }

        if (num > max_val) {
            max_val = num;
        }
    }

    if (!std::in_range<std::size_t>(max_val)) {
        throw std::length_error(
            "TwinArray Sort key range cannot be represented by std::size_t.");
    }

    const std::size_t k = static_cast<std::size_t>(max_val);

    if (k == std::numeric_limits<std::size_t>::max()) {
        throw std::length_error(
            "TwinArray Sort key range is too large for a k + 1 domain.");
    }

    const std::size_t k_plus_1 = k + 1;

    const std::vector<T> value_probe;
    const std::vector<std::size_t> count_probe;

    if (k_plus_1 > value_probe.max_size() ||
        k_plus_1 > count_probe.max_size()) {
        throw std::length_error(
            "TwinArray Sort key domain exceeds std::vector::max_size().");
    }

    return k_plus_1;
}

} // namespace detail

/**
 * @brief Returns a sorted copy using TwinArray Sort.
 *
 * Implements the architecture of Algorithm 1 from the original TwinArray Sort
 * publication for non-negative integer inputs.
 *
 * In the no-duplicate branch, zero is emitted before the non-zero
 * value_store entries. This is semantically equivalent to the paper's
 * reconstruction order while avoiding front insertion into std::vector.
 *
 * @throws std::invalid_argument if a signed input contains a negative value.
 * @throws std::length_error if the indexed k + 1 domain cannot be represented
 *         safely by the required auxiliary vectors.
 *
 * Time complexity: O(n + k), where k = max(arr), for n >= 2.
 * Auxiliary working space: O(k); this copy-returning API also returns O(n)
 * output storage.
 */
template <TwinArrayInteger T>
[[nodiscard]] std::vector<T>
twinarray_sort_copy(const std::vector<T>& arr)
{
    if (arr.empty()) {
        return {};
    }

    if (arr.size() == 1) {
        detail::validate_singleton(arr.front());
        return std::vector<T>{arr.front()};
    }

    const std::size_t k_plus_1 = detail::checked_domain_size(arr);

    std::vector<T> value_store(k_plus_1, T{0});
    std::vector<std::size_t> count_store(k_plus_1, 0);

    bool has_duplicates = false;

    for (const T num : arr) {
        const std::size_t idx = static_cast<std::size_t>(num);
        value_store[idx] = num;

        if (++count_store[idx] > 1) {
            has_duplicates = true;
        }
    }

    std::vector<T> sorted_arr(arr.size());
    auto out = sorted_arr.begin();

    if (!has_duplicates) {
        if (count_store[0] == 1) {
            *out++ = T{0};
        }

        for (const T value : value_store) {
            if (value != T{0}) {
                *out++ = value;
            }
        }
    } else {
        for (std::size_t i = 0; i < k_plus_1; ++i) {
            const std::size_t count = count_store[i];

            if (count > 0) {
                out = std::fill_n(out, count, value_store[i]);
            }
        }
    }

    return sorted_arr;
}

/**
 * @brief Sorts by reconstructing the output directly into the input vector.
 *
 * This avoids a separate sorted output vector, but TwinArray Sort remains an
 * O(k)-auxiliary-space algorithm because value_store and count_store are still
 * required. It is therefore not an in-place sort in the conventional O(1)
 * auxiliary-space sense.
 *
 * @throws std::invalid_argument if a signed input contains a negative value.
 * @throws std::length_error if the indexed k + 1 domain cannot be represented
 *         safely by the required auxiliary vectors.
 *
 * Time complexity: O(n + k), where k = max(arr), for n >= 2.
 * Auxiliary working space: O(k).
 */
template <TwinArrayInteger T>
void twinarray_sort(std::vector<T>& arr)
{
    if (arr.empty()) {
        return;
    }

    if (arr.size() == 1) {
        detail::validate_singleton(arr.front());
        return;
    }

    const std::size_t k_plus_1 = detail::checked_domain_size(arr);

    std::vector<T> value_store(k_plus_1, T{0});
    std::vector<std::size_t> count_store(k_plus_1, 0);

    bool has_duplicates = false;

    for (const T num : arr) {
        const std::size_t idx = static_cast<std::size_t>(num);
        value_store[idx] = num;

        if (++count_store[idx] > 1) {
            has_duplicates = true;
        }
    }

    auto out = arr.begin();

    if (!has_duplicates) {
        if (count_store[0] == 1) {
            *out++ = T{0};
        }

        for (const T value : value_store) {
            if (value != T{0}) {
                *out++ = value;
            }
        }
    } else {
        for (std::size_t i = 0; i < k_plus_1; ++i) {
            const std::size_t count = count_store[i];

            if (count > 0) {
                out = std::fill_n(out, count, value_store[i]);
            }
        }
    }
}

} // namespace twinarray

#endif // TWINARRAY_SORT_HPP
