#!/bin/sh
''''exec python3 -u -- "$0" ${1+"$@"} # '''
# vi: syntax=python

import json

import numpy as np

from typing import Tuple, Set
from numpy.typing import NDArray
from dataclasses import dataclass
from pathlib import Path

from .naive import naive_2d

@dataclass
class TestCase:
    __test__ = False
    text_array: NDArray[np.uint8]
    pattern: NDArray[np.uint8]
    solutions: Set[Tuple[int, int]]

    def save(self, filename: str):
        data = {
            "array": self.text_array.tolist(),
            "pattern": self.pattern.tolist(),
            "solutions": list(self.solutions)
        }

        json_str = json.dumps(data, indent=4)
        json_str = json_str.replace(",\n           ", ",")
        Path(f"{filename}.json").write_text(json_str)


def random_2d_pattern_with_solution_simple(array_size: int, pattern_size: int, seed: int|None = None) -> TestCase:
    if pattern_size > array_size:
        raise ValueError('pattern_size must be smaller than array_size')

    if seed:
        np.random.seed(seed)

    text_array = np.uint8(5) * np.ones(shape=(array_size, array_size), dtype=np.uint8)
    pattern = np.random.randint(0, 4, size=(pattern_size, pattern_size)).astype(np.uint8)
    solutions = set()

    x_max = y_max = array_size - pattern_size
    x, y = 0, 0

    while y < y_max:
        coin_flip = np.random.random()

        if coin_flip < 0.5:
            solutions.add((int(x) + 1, int(y) + 1))
            text_array[x: x + pattern_size, y: y + pattern_size] = pattern

        x += pattern_size + 1

        if x >= x_max:
            x = 0
            y += pattern_size + 1


    return TestCase(text_array, pattern, solutions)


def random_2d_pattern_with_solution(array_size: int, pattern_size: int, seed: int|None = None) -> TestCase:
    if pattern_size > array_size:
        raise ValueError('pattern_size must be smaller than array_size')

    if seed:
        np.random.seed(seed)

    text_array = np.random.randint(0, 4, size=(array_size, array_size)).astype(np.uint8)
    pattern = np.random.randint(0, 4, size=(pattern_size, pattern_size)).astype(np.uint8)
    solutions = naive_2d(text_array.tolist(), pattern.tolist())

    return TestCase(text_array, pattern, solutions)

try:
    import click

    @click.command()
    @click.argument('array_size', type=int)
    @click.argument('pattern_size', type=int)
    @click.argument('filename', type=str)
    @click.option('--seed', type=int, default=None)
    def generator(filename: str, array_size: int, pattern_size: int, seed: int|None) -> None:
        test_case = random_2d_pattern_with_solution(array_size, pattern_size, seed)
        test_case.save(filename)

    if __name__ == '__main__':
        generator()

except ModuleNotFoundError:
    pass
