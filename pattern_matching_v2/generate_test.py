import random
import argparse

def generate_random_row(length, charset):
    if length == 0: return ""
    return "".join(random.choice(charset) for _ in range(length))

def generate_pattern(m1, m2, charset):
    if m1 == 0: return []
    return [generate_random_row(m2, charset) for _ in range(m1)]

def generate_text_background(n1, n2, charset):
    if n1 == 0: return []
    return [generate_random_row(n2, charset) for _ in range(n1)]

def get_lps_for_rows(pattern_rows):
    m1 = len(pattern_rows)
    if m1 == 0:
        return []
    lps = [0] * m1
    length = 0
    i = 1
    while i < m1:
        if pattern_rows[i] == pattern_rows[length]:
            length += 1
            lps[i] = length
            i += 1
        else:
            if length != 0:
                length = lps[length - 1]
            else:
                lps[i] = 0
                i += 1
    return lps

def plant_pattern_in_text(text_rows_list_of_list_of_chars, pattern, r_plant, c_plant):
    m1_p = len(pattern)
    if m1_p == 0:
        return
    m2_p = len(pattern[0])

    for i in range(m1_p):
        for j in range(m2_p):
            if r_plant + i < len(text_rows_list_of_list_of_chars) and \
                    c_plant + j < len(text_rows_list_of_list_of_chars[r_plant + i]):
                text_rows_list_of_list_of_chars[r_plant + i][c_plant + j] = pattern[i][j]

def check_rect_overlap(r1, c1, height1, width1, r2, c2, height2, width2):
    if (c1 + width1 <= c2 or
            c2 + width2 <= c1 or
            r1 + height1 <= r2 or
            r2 + height2 <= r1):
        return False
    return True

def generate_test_case(m1, m2, n1, n2, charset, num_target_occurrences, overlap_strategy):
    if m1 == 0:
        P = []
        T_bg = generate_text_background(n1, n2, charset)
        return P, T_bg, []

    P = generate_pattern(m1, m2, charset)
    T_bg_rows = generate_text_background(n1, n2, charset)

    T_list_of_list_chars = [list(row) for row in T_bg_rows]

    planted_occurrences_coords = []

    lps_P_rows = get_lps_for_rows(P)
    kmp_overlap_vertical_shift = 0
    if m1 > 0 and lps_P_rows[m1 - 1] > 0:
        kmp_overlap_vertical_shift = m1 - lps_P_rows[m1 - 1]

    possible_initial_coords = []
    if m1 > 0 :
        for r in range(n1 - m1 + 1):
            for c in range(n2 - m2 + 1):
                possible_initial_coords.append((r, c))

    random.shuffle(possible_initial_coords)

    if not possible_initial_coords and num_target_occurrences > 0:
        T_final_rows = ["".join(row_chars) for row_chars in T_list_of_list_chars]
        return P, T_final_rows, []


    if overlap_strategy == "kmp_driven" and kmp_overlap_vertical_shift > 0 and \
            num_target_occurrences >= 2 and len(possible_initial_coords) > 0:

        r1, c1 = possible_initial_coords.pop(0)
        plant_pattern_in_text(T_list_of_list_chars, P, r1, c1)
        planted_occurrences_coords.append((r1, c1))

        r2, c2 = r1 + kmp_overlap_vertical_shift, c1
        if (r2 + m1 <= n1 and c2 + m2 <= n2):
            if (r2,c2) != (r1,c1):
                plant_pattern_in_text(T_list_of_list_chars, P, r2, c2)
                if (r2,c2) not in planted_occurrences_coords:
                    planted_occurrences_coords.append((r2, c2))
                if (r2,c2) in possible_initial_coords:
                    possible_initial_coords.remove((r2,c2))

    num_already_planted = len(planted_occurrences_coords)
    num_to_plant_additionally = num_target_occurrences - num_already_planted

    additional_planted_this_loop = 0

    coords_candidates_for_loop = list(possible_initial_coords)

    for r_cand, c_cand in coords_candidates_for_loop:
        if additional_planted_this_loop >= num_to_plant_additionally:
            break
        if len(planted_occurrences_coords) >= num_target_occurrences:
            break

        can_plant = True
        if overlap_strategy == "none":
            for r_prev, c_prev in planted_occurrences_coords:
                if check_rect_overlap(r_cand, c_cand, m1, m2, r_prev, c_prev, m1, m2):
                    can_plant = False
                    break

        if can_plant:
            if (r_cand, c_cand) not in planted_occurrences_coords:
                plant_pattern_in_text(T_list_of_list_chars, P, r_cand, c_cand)
                planted_occurrences_coords.append((r_cand, c_cand))
                additional_planted_this_loop += 1

    planted_occurrences_coords.sort()

    T_final_rows = ["".join(row_chars) for row_chars in T_list_of_list_chars]

    output_occurrences_1_indexed = sorted([(r + 1, c + 1) for r, c in planted_occurrences_coords])
    return P, T_final_rows, output_occurrences_1_indexed


def main():
    parser = argparse.ArgumentParser(description="Generate test cases for 2D KMP-like algorithm.")
    parser.add_argument("--m1", type=int, default=2, help="Number of rows in pattern P")
    parser.add_argument("--m2", type=int, default=2, help="Number of columns in pattern P")
    parser.add_argument("--n1", type=int, default=8, help="Number of rows in text T")
    parser.add_argument("--n2", type=int, default=8, help="Number of columns in text T")
    parser.add_argument("--charset", type=str, default="AB", help="Characters to use for generation")
    parser.add_argument("--num_occurrences", type=int, default=2, help="Target number of P occurrences to plant in T")
    parser.add_argument("--overlap_strategy", type=str, choices=["random", "kmp_driven", "none"], default="random",
                        help="Strategy for planting patterns")
    parser.add_argument("--seed", type=int, default=None, help="Random seed for reproducibility")
    parser.add_argument("--output_file", type=str, default=None, help="File to write the test case to")

    args = parser.parse_args()

    if args.seed is not None:
        random.seed(args.seed)

    if args.m1 < 0 or args.m2 < 0 or args.n1 < 0 or args.n2 < 0 :
        print("Error: Dimensions (m1, m2, n1, n2) must be non-negative.")
        return
    if args.m1 == 0 and args.num_occurrences > 0 :
        print("Warning: m1 is 0 (no pattern rows). Setting num_occurrences to 0.")
        args.num_occurrences = 0
    if args.m1 > 0 and args.m2 == 0 and args.num_occurrences > 0:
        print("Warning: Pattern has m2=0 (0 columns).")

    P, T, expected_occurrences = generate_test_case(
        args.m1, args.m2, args.n1, args.n2,
        args.charset, args.num_occurrences, args.overlap_strategy
    )

    output_lines = []
    output_lines.append(f"{args.m1} {args.m2} {args.n1} {args.n2}")
    for p_row in P:
        output_lines.append(p_row)
    for t_row in T:
        output_lines.append(t_row)

    solution_lines = []
    solution_lines.append(f"\n# --- Expected Solution (Intentionally Planted Occurrences) ---")
    solution_lines.append(f"# Count: {len(expected_occurrences)}")
    for r_out, c_out in expected_occurrences:
        solution_lines.append(f"# {r_out} {c_out}")

    if args.output_file:
        with open(args.output_file, 'w') as f:
            for line in output_lines:
                f.write(line + '\n')
            for line in solution_lines:
                f.write(line + '\n')
        print(f"Test case input and expected solution comments written to {args.output_file}")
    else:
        for line in output_lines:
            print(line)
        for line in solution_lines:
            print(line)

if __name__ == "__main__":
    main()