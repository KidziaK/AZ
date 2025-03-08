import pytest
from .pattern_generator import random_2d_pattern_with_solution_simple, TestCase
from.naive import naive_2d

test_cases = [random_2d_pattern_with_solution_simple(n, m) for n in range(4, 20) for m in range(2, n)]

@pytest.mark.parametrize("test_case", test_cases)
def test_naive(test_case: TestCase) -> None:
    solution = naive_2d(test_case.text_array.tolist(), test_case.pattern.tolist())
    assert solution == test_case.solutions
