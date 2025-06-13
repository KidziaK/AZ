#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <ctime>

std::vector<std::string> random_matrix(int rows, int cols, const std::string& alphabet, std::mt19937& rng) {
    std::vector<std::string> mat(rows);
    std::uniform_int_distribution<> dist(0, alphabet.size() - 1);
    for (int i = 0; i < rows; ++i) {
        mat[i].resize(cols);
        for (int j = 0; j < cols; ++j) {
            mat[i][j] = alphabet[dist(rng)];
        }
    }
    return mat;
}

int main(int argc, char* argv[]) {
    int ph = 3, pw = 3, th = 5, tw = 5;
    std::string alphabet = "abc";
    unsigned int seed = static_cast<unsigned int>(std::time(nullptr));
    std::string output_file;

    if (argc > 1) ph = std::stoi(argv[1]);
    if (argc > 2) pw = std::stoi(argv[2]);
    if (argc > 3) th = std::stoi(argv[3]);
    if (argc > 4) tw = std::stoi(argv[4]);
    if (argc > 5) alphabet = argv[5];
    if (argc > 6) seed = static_cast<unsigned int>(std::stoul(argv[6]));
    if (argc > 7) output_file = argv[7];

    std::mt19937 rng(seed);
    auto pattern = random_matrix(ph, pw, alphabet, rng);
    auto text = random_matrix(th, tw, alphabet, rng);

    std::ostream* out = &std::cout;
    std::ofstream fout;
    if (!output_file.empty()) {
        fout.open(output_file);
        if (!fout) {
            std::cerr << "Failed to open output file: " << output_file << std::endl;
            return 1;
        }
        out = &fout;
    }

    *out << ph << " " << pw << " " << th << " " << tw << "\n";
    for (const auto& row : pattern) *out << row << "\n";
    for (const auto& row : text) *out << row << "\n";

    return 0;
}
