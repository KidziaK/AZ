#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "naive.h"

using json = nlohmann::json;

int main() {
    std::ifstream f("../../test_cases/2_100_0.json");
    json j = json::parse(f);

    auto input_array = j["array"].get<Matrix<char>>();
    auto pattern = j["pattern"].get<Matrix<char>>();
    auto solutions = j["solutions"].get<SolutionSet>();

    auto result = solve_2d_naive(input_array, pattern);

    std::cout << (result == solutions) << std::endl;
}
