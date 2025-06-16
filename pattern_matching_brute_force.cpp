#include <iostream>
#include <vector>
#include <string>

int main() {
    int ph, pw, th, tw;
    std::cin >> ph >> pw >> th >> tw;
    std::cin.ignore();

    std::vector<std::string> pattern(ph);
    for (int i = 0; i < ph; ++i) {
        std::getline(std::cin, pattern[i]);
    }

    std::vector<std::string> text(th);
    for (int i = 0; i < th; ++i) {
        std::getline(std::cin, text[i]);
    }

    std::vector<std::pair<int, int>> results;
    for (int i = 0; i <= th - ph; ++i) {
        for (int j = 0; j <= tw - pw; ++j) {
            bool match = true;
            for (int pi = 0; pi < ph && match; ++pi) {
                for (int pj = 0; pj < pw; ++pj) {
                    if (text[i + pi][j + pj] != pattern[pi][pj]) {
                        match = false;
                        break;
                    }
                }
            }
            if (match) {
                results.emplace_back(i + 1, j + 1); // 1-based
            }
        }
    }

    std::cout << results.size() << std::endl;
    for (const auto& [row, col] : results) {
        std::cout << row << " " << col << std::endl;
    }
    return 0;
}
