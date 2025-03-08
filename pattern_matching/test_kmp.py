from typing import Tuple, List

import pytest
from .kpm import kmp_1d

def test_search_no_solution():
    intput_string = "ac"
    pattern = "ab"

    assert len(kmp_1d(intput_string, pattern)) == 0

def test_search_zero_length():
    intput_string = "ac"
    pattern = ""

    with pytest.raises(ValueError):
        _ = kmp_1d(intput_string, pattern)

test_cases = [
    (("aaab", "aab"), (1,)),
    (("qwebqwebqwe", "qwe"), (0, 4, 8)),
    (("adsgwadsxdsgwadsgz", "dsgwadsgz"), (9,))
]

@pytest.mark.parametrize("test_input, expected", test_cases)
def test_search_parametrized(test_input: Tuple[str, str], expected: Tuple[int, ...]):
    intput_string, pattern = test_input
    assert kmp_1d(intput_string, pattern) == expected
