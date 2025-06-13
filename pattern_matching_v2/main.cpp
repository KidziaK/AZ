// Optimized Baker-Bird Algorithm Implementation (Corrected for segmentation fault)
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <chrono>

struct ACNode {
    std::unordered_map<char, int> children;
    int parent = -1;
    char parent_char = 0;
    int failure_link = 0;
    int output_link = 0;
    std::vector<int> pattern_indices;
    bool is_terminal = false;

    ACNode(int p = -1, char ch = 0) : parent(p), parent_char(ch) {}
};

std::vector<ACNode> ac_nodes;
std::vector<std::string> pattern_rows_global;

inline void add_string(const std::string& s, int pattern_index) {
    int current_node_idx = 0;
    for (char ch : s) {
        auto& children = ac_nodes[current_node_idx].children;
        auto it = children.find(ch);
        if (it == children.end()) {
            ac_nodes.emplace_back(current_node_idx, ch);
            children[ch] = (int)ac_nodes.size() - 1;
        }
        current_node_idx = children[ch];
    }
    ACNode& node = ac_nodes[current_node_idx];
    node.is_terminal = true;
    node.pattern_indices.push_back(pattern_index);
}

void build_failure_links() {
    std::queue<int> q;
    for (const auto& [key, val] : ac_nodes[0].children) {
        q.push(val);
        ac_nodes[val].failure_link = 0;
        ac_nodes[val].output_link = ac_nodes[0].is_terminal ? 0 : ac_nodes[0].output_link;
    }

    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (const auto& [ch, v] : ac_nodes[u].children) {
            int j = ac_nodes[u].failure_link;
            while (j && !ac_nodes[j].children.count(ch)) {
                j = ac_nodes[j].failure_link;
            }
            if (ac_nodes[j].children.count(ch)) {
                ac_nodes[v].failure_link = ac_nodes[j].children[ch];
            } else {
                ac_nodes[v].failure_link = 0;
            }
            int fl = ac_nodes[v].failure_link;
            ac_nodes[v].output_link = ac_nodes[fl].is_terminal ? fl : ac_nodes[fl].output_link;
            q.push(v);
        }
    }
}

inline int get_next_state(int current_state_idx, char ch) {
    while (current_state_idx && !ac_nodes[current_state_idx].children.count(ch)) {
        current_state_idx = ac_nodes[current_state_idx].failure_link;
    }
    auto it = ac_nodes[current_state_idx].children.find(ch);
    return (it != ac_nodes[current_state_idx].children.end()) ? it->second : 0;
}

inline void get_matched_indices(int state_idx, std::vector<int>& matched) {
    int current_output_node = state_idx;
    while (current_output_node != 0) {
        if (current_output_node >= (int)ac_nodes.size()) break;
        const ACNode& node = ac_nodes[current_output_node];
        if (node.is_terminal) {
            for (int idx : node.pattern_indices) {
                if (idx >= 0 && idx < (int)pattern_rows_global.size()) {
                    matched.push_back(idx);
                }
            }
        }
        current_output_node = node.output_link;
    }
}

void PRE_AC(const std::vector<std::string>& P) {
    ac_nodes.clear();
    ac_nodes.emplace_back();
    pattern_rows_global = P;
    for (int i = 0; i < (int)P.size(); ++i) {
        add_string(P[i], i);
    }
    build_failure_links();
}

void PRE_KMP_2D(const std::vector<std::string>& P, int m1, std::vector<int>& next_row) {
    next_row.assign(m1, 0);
    int length = 0;
    for (int i = 1; i < m1; ++i) {
        while (length > 0 && P[i] != P[length]) {
            length = next_row[length - 1];
        }
        if (P[i] == P[length]) ++length;
        next_row[i] = length;
    }
}

std::vector<std::pair<int, int>> PATTERN_2D_KMP_AC(
    const std::vector<std::string>& T,
    const std::vector<std::string>& P,
    int n1, int n2, int m1, int m2)
{
    if (m1 == 0 || m2 == 0 || n1 < m1 || n2 < m2) return {};

    PRE_AC(P);
    std::vector<int> next_row;
    PRE_KMP_2D(P, m1, next_row);

    std::vector<int> match_length(n2 - m2 + 1, 0);
    std::vector<std::pair<int, int>> results;

    // auto t_start = std::chrono::high_resolution_clock::now();
    std::vector<int> matched_row_indices;
    for (int i = 0; i < n1; ++i) {
        int current_state_idx = 0;
        for (int j = 0; j < n2; ++j) {
            current_state_idx = get_next_state(current_state_idx, T[i][j]);
            int c = j - m2 + 1;
            if (c >= 0 && c < (int)match_length.size()) {
                matched_row_indices.clear();
                get_matched_indices(current_state_idx, matched_row_indices);

                if (matched_row_indices.empty()) {
                    match_length[c] = 0;
                } else {
                    int best = 0;
                    for (int k : matched_row_indices) {
                        if (k >= (int)pattern_rows_global.size()) continue;
                        int temp = match_length[c];
                        while (temp > 0 && pattern_rows_global[k] != pattern_rows_global[temp]) {
                            temp = next_row[temp - 1];
                        }
                        if (pattern_rows_global[k] == pattern_rows_global[temp]) ++temp;
                        if (temp > best) best = temp;
                    }
                    match_length[c] = best;
                    if (best == m1) {
                        results.emplace_back(i - m1 + 1, c);
                        match_length[c] = next_row[m1 - 1];
                    }
                }
            }
        }
    }
    // auto t_end = std::chrono::high_resolution_clock::now();
    // std::cout << "Segment name took "
    //           << std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count()
    //           << " ms\n";
    return results;
}

int main(int argc, char* argv[]) {
    //std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>\n";
        return 1;
    }
    std::ifstream input_file(argv[1]);
    if (!input_file.is_open()) {
        std::cerr << "Error opening input file: " << argv[1] << "\n";
        return 1;
    }

    std::ofstream output_file(argv[2]);
    if (!output_file.is_open()) {
        std::cerr << "Error opening output file: " << argv[2] << "\n";
        return 1;
    }

    int n1, n2, m1, m2;
    input_file >> m1 >> m2 >> n1 >> n2;
    if (!input_file) return 1;
    std::vector<std::string> P(m1);
    for (int i = 0; i < m1; ++i) input_file >> P[i];
    std::vector<std::string> T(n1);
    for (int i = 0; i < n1; ++i) input_file >> T[i];
    input_file.close();
    std::vector<std::pair<int, int>> occurrences = PATTERN_2D_KMP_AC(T, P, n1, n2, m1, m2);
    output_file << occurrences.size() << "\n";
    for (const auto& [r, c] : occurrences) {
        output_file << r + 1 << " " << c + 1 << "\n";
    }

    return 0;
}
