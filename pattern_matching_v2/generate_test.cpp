#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <fstream>
#include <utility>
#include <optional>
#include <set>
#include <map>
#include <stdexcept>

using Matrix = std::vector<std::string>;
using Coords = std::pair<int, int>;

std::string generate_random_row(int length, const std::string& charset, std::mt19937& gen) {
    if (length == 0) return "";
    std::uniform_int_distribution<> distrib(0, charset.length() - 1);
    std::string row;
    row.reserve(length);
    for (int i = 0; i < length; ++i) {
        row += charset[distrib(gen)];
    }
    return row;
}

Matrix generate_matrix(int rows, int cols, const std::string& charset, std::mt19937& gen) {
    if (rows == 0) return {};
    Matrix mat;
    mat.reserve(rows);
    for (int i = 0; i < rows; ++i) {
        mat.push_back(generate_random_row(cols, charset, gen));
    }
    return mat;
}

std::vector<int> get_lps_for_rows(const Matrix& pattern_rows) {
    int m1 = pattern_rows.size();
    if (m1 == 0) return {};
    
    std::vector<int> lps(m1, 0);
    int length = 0;
    int i = 1;
    while (i < m1) {
        if (pattern_rows[i] == pattern_rows[length]) {
            length++;
            lps[i] = length;
            i++;
        } else {
            if (length != 0) {
                length = lps[length - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

void plant_pattern_in_text(Matrix& text, const Matrix& pattern, int r_plant, int c_plant) {
    if (pattern.empty()) return;
    
    int m1_p = pattern.size();
    int m2_p = pattern[0].size();
    
    for (int i = 0; i < m1_p; ++i) {
        int target_row = r_plant + i;
        if (target_row >= text.size()) continue;
        for (int j = 0; j < m2_p; ++j) {
            int target_col = c_plant + j;
            if (target_col < text[target_row].size()) {
                text[target_row][target_col] = pattern[i][j];
            }
        }
    }
}

bool check_rect_overlap(int r1, int c1, int h1, int w1, int r2, int c2, int h2, int w2) {
    if (c1 + w1 <= c2 || c2 + w2 <= c1 || r1 + h1 <= r2 || r2 + h2 <= r1) {
        return false;
    }
    return true;
}

struct TestCaseResult {
    Matrix P;
    Matrix T;
    std::vector<Coords> occurrences;
};

TestCaseResult generate_test_case(int m1, int m2, int n1, int n2, const std::string& charset,
                                    int num_target_occurrences, const std::string& overlap_strategy, std::mt19937& gen) {
    if (m1 == 0) {
        Matrix T_bg = generate_matrix(n1, n2, charset, gen);
        return {{}, T_bg, {}};
    }

    Matrix P = generate_matrix(m1, m2, charset, gen);
    Matrix T = generate_matrix(n1, n2, charset, gen);
    std::vector<Coords> planted_occurrences_coords;
    std::set<Coords> used_coords;

    std::vector<Coords> possible_initial_coords;
    if (m1 > 0 && n1 >= m1 && n2 >= m2) {
        for (int r = 0; r <= n1 - m1; ++r) {
            for (int c = 0; c <= n2 - m2; ++c) {
                possible_initial_coords.emplace_back(r, c);
            }
        }
    }
    std::shuffle(possible_initial_coords.begin(), possible_initial_coords.end(), gen);

    if (possible_initial_coords.empty() && num_target_occurrences > 0) {
        return {P, T, {}};
    }

    if (overlap_strategy == "kmp_driven" && num_target_occurrences >= 2 && !possible_initial_coords.empty()) {
        auto lps_P_rows = get_lps_for_rows(P);
        int kmp_overlap_vertical_shift = m1 > 0 && lps_P_rows.back() > 0 ? m1 - lps_P_rows.back() : 0;
        
        if (kmp_overlap_vertical_shift > 0) {
            Coords r1c1 = possible_initial_coords.front();
            plant_pattern_in_text(T, P, r1c1.first, r1c1.second);
            planted_occurrences_coords.push_back(r1c1);
            used_coords.insert(r1c1);

            Coords r2c2 = {r1c1.first + kmp_overlap_vertical_shift, r1c1.second};
            if (r2c2.first + m1 <= n1 && r2c2.second + m2 <= n2 && used_coords.find(r2c2) == used_coords.end()) {
                plant_pattern_in_text(T, P, r2c2.first, r2c2.second);
                planted_occurrences_coords.push_back(r2c2);
                used_coords.insert(r2c2);
            }
        }
    }

    for (const auto& coord_cand : possible_initial_coords) {
        if (planted_occurrences_coords.size() >= num_target_occurrences) break;
        if (used_coords.count(coord_cand)) continue;

        bool can_plant = true;
        if (overlap_strategy == "none") {
            for (const auto& prev_coord : planted_occurrences_coords) {
                if (check_rect_overlap(coord_cand.first, coord_cand.second, m1, m2, 
                                        prev_coord.first, prev_coord.second, m1, m2)) {
                    can_plant = false;
                    break;
                }
            }
        }
        
        if (can_plant) {
            plant_pattern_in_text(T, P, coord_cand.first, coord_cand.second);
            planted_occurrences_coords.push_back(coord_cand);
            used_coords.insert(coord_cand);
        }
    }

    std::sort(planted_occurrences_coords.begin(), planted_occurrences_coords.end());
    std::vector<Coords> output_occurrences_1_indexed;
    for (const auto& coord : planted_occurrences_coords) {
        output_occurrences_1_indexed.emplace_back(coord.first + 1, coord.second + 1);
    }
    
    return {P, T, output_occurrences_1_indexed};
}

int main(int argc, char* argv[]) {
    std::map<std::string, std::string> args;
    args["--m1"] = "2";
    args["--m2"] = "2";
    args["--n1"] = "8";
    args["--n2"] = "8";
    args["--charset"] = "AB";
    args["--num_occurrences"] = "2";
    args["--overlap_strategy"] = "random";
    args["--seed"] = "";
    args["--output_file"] = "";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (args.count(arg) && i + 1 < argc) {
            args[arg] = argv[++i];
        }
    }
    
    int m1, m2, n1, n2, num_occurrences;
    try {
        m1 = std::stoi(args["--m1"]);
        m2 = std::stoi(args["--m2"]);
        n1 = std::stoi(args["--n1"]);
        n2 = std::stoi(args["--n2"]);
        num_occurrences = std::stoi(args["--num_occurrences"]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Invalid number provided for dimensions or occurrences." << std::endl;
        return 1;
    }
    
    std::string charset = args["--charset"];
    std::string overlap_strategy = args["--overlap_strategy"];
    std::optional<unsigned int> seed;
    if (!args["--seed"].empty()) {
        try {
            seed = std::stoul(args["--seed"]);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error: Invalid number provided for seed." << std::endl;
            return 1;
        }
    }
    std::string output_file = args["--output_file"];

    if (m1 < 0 || m2 < 0 || n1 < 0 || n2 < 0) {
        std::cerr << "Error: Dimensions (m1, m2, n1, n2) must be non-negative." << std::endl;
        return 1;
    }
    if (m1 == 0 && num_occurrences > 0) {
        std::cerr << "Warning: m1 is 0 (no pattern rows). Setting num_occurrences to 0." << std::endl;
        num_occurrences = 0;
    }
    if (m1 > 0 && m2 == 0 && num_occurrences > 0) {
        std::cerr << "Warning: Pattern has m2=0 (0 columns)." << std::endl;
    }

    std::mt19937 gen;
    if (seed.has_value()) {
        gen.seed(seed.value());
    } else {
        std::random_device rd;
        gen.seed(rd());
    }

    TestCaseResult result = generate_test_case(m1, m2, n1, n2, charset, num_occurrences, overlap_strategy, gen);
    
    std::vector<std::string> output_lines;
    output_lines.push_back(std::to_string(m1) + " " + std::to_string(m2) + " " + std::to_string(n1) + " " + std::to_string(n2));
    for (const auto& p_row : result.P) {
        output_lines.push_back(p_row);
    }
    for (const auto& t_row : result.T) {
        output_lines.push_back(t_row);
    }

    std::vector<std::string> solution_lines;
    solution_lines.push_back("\n# --- Expected Solution (Intentionally Planted Occurrences) ---");
    solution_lines.push_back("# Count: " + std::to_string(result.occurrences.size()));
    for (const auto& coord : result.occurrences) {
        solution_lines.push_back("# " + std::to_string(coord.first) + " " + std::to_string(coord.second));
    }

    if (!output_file.empty()) {
        std::ofstream outfile(output_file);
        if (!outfile) {
            std::cerr << "Error: Could not open file for writing: " << output_file << std::endl;
            return 1;
        }
        for (const auto& line : output_lines) {
            outfile << line << '\n';
        }
        for (const auto& line : solution_lines) {
            outfile << line << '\n';
        }
        std::cout << "Test case input and expected solution comments written to " << output_file << std::endl;
    } else {
        for (const auto& line : output_lines) {
            std::cout << line << '\n';
        }
        for (const auto& line : solution_lines) {
            std::cout << line << '\n';
        }
    }

    return 0;
}
