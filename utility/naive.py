from typing import List, Tuple, Set
from itertools import product

def naive_2d(text_array: List[List[int]], pattern: List[List[int]]) -> Set[Tuple[int, int]]:
    if len(text_array) == 0 or len(text_array[0]) == 0:
        raise ValueError(f'Dimensions in the text_array have to be positive. Provided {text_array}')

    if len(pattern) == 0 or len(pattern[0]) == 0:
        raise ValueError(f'Dimensions in the pattern have to be positive. Provided {pattern}')

    rows, cols = len(text_array), len(text_array[0])
    rows_pattern, cols_pattern = len(pattern), len(pattern[0])

    solutions = set()

    for i, j in product(range(rows - rows_pattern), range(cols - cols_pattern)):
        solution_found = True

        for k, l in product(range(rows_pattern), range(cols_pattern)):
            value_text = text_array[i + k][j + l]
            value_pattern = pattern[k][l]

            if value_text != value_pattern:
                solution_found = False
                break

        if solution_found:
            solutions.add((i + 1, j + 1))

    return solutions
