from utility.pattern_generator import random_2d_pattern_with_solution
from pathlib import Path

TEST_CASES_FOLDER = Path('test_cases')

if __name__ == '__main__':
    TEST_CASES_FOLDER.mkdir(exist_ok=True)

    for array_size in [100, 200, 300, 400, 500]:
        for pattern_size in range(2, 4):
            for seed in range(10):
                test_case = random_2d_pattern_with_solution(array_size, pattern_size, seed)
                test_case.save(f"{TEST_CASES_FOLDER}/{pattern_size}_{array_size}_{seed}")