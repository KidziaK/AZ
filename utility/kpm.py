from typing import List, Tuple


def construct_lps(pattern: str) -> List[int]:
    i, j = 0, 1
    longest_prefix_suffix = 0
    lps_table = [0] * len(pattern)

    while j < len(pattern):
        if pattern[i] == pattern[j]:
            longest_prefix_suffix += 1
            lps_table[j] = longest_prefix_suffix
            i += 1
        else:
            i = lps_table[longest_prefix_suffix] + 1
            lps_table[j] = i
            longest_prefix_suffix = 0
        j += 1

    return lps_table



def kmp_1d(input_string: str, pattern: str) -> Tuple[int, ...]:
    if len(pattern) == 0:
        raise ValueError("Pattern cannot be empty")

    lps = construct_lps(pattern)

    i, j = 0, 0
    solutions = []

    while i < len(input_string):
        if input_string[i] == pattern[j]:
            j += 1
        else:
            j = lps[j]

        i += 1

        if j == len(pattern):
            solutions.append(i - j)
            j = lps[j - 1]

    return tuple(solutions)
