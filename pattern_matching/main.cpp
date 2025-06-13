#include <iostream>
#include <vector>
#include "naive.h"

int main() {
    int ph, pw, th, tw;
    std::cin >> ph >> pw >> th >> tw;
    std::cin.ignore(); // skip to next line

    Matrix<char> pattern(ph, std::vector<char>(pw));
    for (int i = 0; i < ph; ++i) {
        std::string line;
        std::getline(std::cin, line);
        for (int j = 0; j < pw; ++j) {
            pattern[i][j] = line[j];
        }
    }

    Matrix<char> input_array(th, std::vector<char>(tw));
    for (int i = 0; i < th; ++i) {
        std::string line;
        std::getline(std::cin, line);
        for (int j = 0; j < tw; ++j) {
            input_array[i][j] = line[j];
        }
    }

    auto result = solve_2d_naive(input_array, pattern);

    for (const auto& [row, col] : result) {
        std::cout << row << " " << col << std::endl;
    }
    return 0;
}
