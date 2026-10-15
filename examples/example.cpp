/*
 * TwinArray Sort example
 * Copyright 2026 Amin Amini
 * SPDX-License-Identifier: Apache-2.0
 */

#include "twinarray_sort.hpp"

#include <iostream>
#include <vector>

int main()
{
    std::vector<int> values{9, 3, 7, 1, 3, 0, 8, 2};

    twinarray::twinarray_sort(values);

    std::cout << "Sorted: ";
    for (const int value : values) {
        std::cout << value << ' ';
    }
    std::cout << '\n';

    return 0;
}
