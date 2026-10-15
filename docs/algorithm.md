# TwinArray Sort: Algorithm and C++20 Correspondence

This document describes the reference implementation of TwinArray Sort from:

> Amin Amini, “TwinArray Sort: An Ultrarapid Conditional Non-Comparison Integer
> Sorting Algorithm”, *Electronics*, 15(3), 609, 2026.  
> https://doi.org/10.3390/electronics15030609

## Input Domain

The original algorithm operates on non-negative integer values.

Let:

- `n` be the number of input elements;
- `k = max(A)` be the maximum input key for a non-empty, non-singleton input.

Negative values are outside the domain of the original algorithm and are
rejected by the C++ API.

## Twin Auxiliary Stores

For inputs requiring sorting, the implementation allocates two zero-initialised
arrays of length `k + 1`:

- `value_store`: stores an encountered key at its corresponding index;
- `count_store`: stores the frequency of each key.

The C++ implementation uses `std::size_t` for counts so frequency storage is
not artificially limited by a narrow input element type.

## Mapping Pass

Each input value is mapped to its index:

```cpp
value_store[idx] = num;

if (++count_store[idx] > 1) {
    has_duplicates = true;
}
```

The Boolean `has_duplicates` determines which reconstruction path is used.

## Unique-Data Reconstruction

When no duplicate exists, present positive keys appear as non-zero entries of
`value_store` in ascending index order.

Key `0` is special because an unused zero-initialised slot and a stored zero
have the same value. Its presence is determined from `count_store[0]`.

The paper handles zero after collecting the positive keys. In this C++ version,
zero is emitted immediately before the positive scan. Since zero is the
smallest valid key, the sorted sequence is identical and no front insertion is
needed.

## Duplicate-Data Reconstruction

When duplicates exist, reconstruction scans indices from `0` through `k`.
For every index with a positive count, the stored value is emitted exactly
`count_store[i]` times.

## Complexity

For normal non-singleton input:

- time: `O(n + k)`;
- auxiliary working storage: `O(k)`.

The copy-returning API also returns a vector of length `n`, so its total
auxiliary/output storage is `O(n + k)`, matching the output-array accounting
used in the original paper.

The direct-output API writes the reconstructed sequence back into the input
vector and therefore avoids a separate `O(n)` output allocation. It still
requires the two `O(k)` auxiliary stores and is not an `O(1)`-space in-place
sorting algorithm.

## Sparse Key Domains

Because both auxiliary storage and domain scanning scale with `k`, inputs with a
very large maximum value relative to `n` can require substantial memory and
time. This characteristic should be considered when choosing datasets for the
algorithm and when interpreting benchmark results.
