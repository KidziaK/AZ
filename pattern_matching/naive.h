#pragma once

#include "common.h"

template <typename T>
SolutionSet solve_2d_naive(const Matrix<T>& input_array, const Matrix<T>& pattern) {
    SolutionSet solutions;
    // SolutionSet is an alias for unordered_set (hash set). reserve() in this case will set the number of
    // hash bins. For 99% of problems this amount will suffice to hold all solutions.
    solutions.reserve(input_array.size());

    size_t n = input_array.size();
    size_t np = input_array[0].size();

    size_t m = pattern.size();
    size_t mp = pattern[0].size();

    for (size_t i = 0; i < n - m; i++) {
        for (size_t j = 0; j < np - mp; j++) {
            bool found = true;

            for (size_t k = 0; k < m; k++) {
                for (size_t l = 0; l < mp; l++) {
                    if (input_array[i + k][j + l] != pattern[k][l]) {
                        found = false;
                        break;
                    }
                }

                if (!found) {
                    break;
                }
            }

            if (found) {
                solutions.emplace(i + 1, j + 1);
            }
        }
    }

    return solutions;
}
