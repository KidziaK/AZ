#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <vector>
#include <utility>
#include <fstream>

struct ACNode {
    std::map<char, int> children;
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

void add_string(const std::string& s, int pattern_index) {
    int current_node_idx = 0;
    for (char ch : s) {
        if (ac_nodes[current_node_idx].children.find(ch) == ac_nodes[current_node_idx].children.end()) {
            ac_nodes.emplace_back(current_node_idx, ch);
            ac_nodes[current_node_idx].children[ch] = ac_nodes.size() - 1;
        }
        current_node_idx = ac_nodes[current_node_idx].children[ch];
    }
    ac_nodes[current_node_idx].is_terminal = true;
    ac_nodes[current_node_idx].pattern_indices.push_back(pattern_index);
}

void build_failure_links() {
    std::queue<int> q;

    for (auto const& [key, val] : ac_nodes[0].children) {
        q.push(val);
        ac_nodes[val].failure_link = 0;
        ac_nodes[val].output_link = ac_nodes[ac_nodes[val].failure_link].is_terminal ? ac_nodes[val].failure_link : ac_nodes[ac_nodes[val].failure_link].output_link;
    }

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (auto const& [ch, v] : ac_nodes[u].children) {
            int j = ac_nodes[u].failure_link;

            while (ac_nodes[j].children.find(ch) == ac_nodes[j].children.end() && j != 0) {
                j = ac_nodes[j].failure_link;
            }

            if (ac_nodes[j].children.find(ch) != ac_nodes[j].children.end()) {
                ac_nodes[v].failure_link = ac_nodes[j].children.at(ch);
            } else {
                ac_nodes[v].failure_link = 0;
            }

             ac_nodes[v].output_link = ac_nodes[ac_nodes[v].failure_link].is_terminal ? ac_nodes[v].failure_link : ac_nodes[ac_nodes[v].failure_link].output_link;

            q.push(v);
        }
    }
}

int get_next_state(int current_state_idx, char ch) {
    while (ac_nodes[current_state_idx].children.find(ch) == ac_nodes[current_state_idx].children.end() && current_state_idx != 0) {
        current_state_idx = ac_nodes[current_state_idx].failure_link;
    }
    if (ac_nodes[current_state_idx].children.find(ch) != ac_nodes[current_state_idx].children.end()) {
        return ac_nodes[current_state_idx].children.at(ch);
    }
    return 0;
}

std::vector<int> get_matched_indices(int state_idx) {
    std::vector<int> matched;
    int current_output_node = state_idx;

    while(current_output_node != 0) {
         if (ac_nodes[current_output_node].is_terminal) {
              matched.insert(matched.end(), ac_nodes[current_output_node].pattern_indices.begin(), ac_nodes[current_output_node].pattern_indices.end());
         }
         current_output_node = ac_nodes[current_output_node].output_link;
    }
    return matched;
}

void PRE_AC(const std::vector<std::string>& P) {
    ac_nodes.clear();
    ac_nodes.emplace_back();
    pattern_rows_global = P;

    for (int i = 0; i < P.size(); ++i) {
        add_string(P[i], i);
    }
    build_failure_links();
}

void PRE_KMP_2D(const std::vector<std::string>& P, int m1, std::vector<int>& next_row) {
    if (m1 == 0) return;
    next_row.assign(m1, 0);

    int length = 0;

    for (int i = 1; i < m1; ++i) {
        while (length > 0 && pattern_rows_global[i] != pattern_rows_global[length]) {
            length = next_row[length - 1];
        }
        if (pattern_rows_global[i] == pattern_rows_global[length]) {
            length++;
        }
        next_row[i] = length;
    }
}

std::vector<std::pair<int, int>> PATTERN_2D_KMP_AC(
    const std::vector<std::string>& T,
    const std::vector<std::string>& P,
    int n1, int n2, int m1, int m2)
{
    if (m1 == 0 || m2 == 0 || n1 < m1 || n2 < m2) {
        return {};
    }

    PRE_AC(P);

    std::vector<int> next_row;
    PRE_KMP_2D(P, m1, next_row);

    std::vector<int> match_length(n2 - m2 + 1, 0);
    std::vector<std::pair<int, int>> results;

    std::vector<int> current_row_match_length(n2 - m2 + 1);

    for (int i = 0; i < n1; ++i) {
        std::fill(current_row_match_length.begin(), current_row_match_length.end(), 0);
        int current_ac_state_for_row_i = 0;

        for (int j = 0; j < n2; ++j) {
            current_ac_state_for_row_i = get_next_state(current_ac_state_for_row_i, T[i][j]);
            std::vector<int> matched_row_indices = get_matched_indices(current_ac_state_for_row_i);

            if (matched_row_indices.empty()) {
                continue;
            }

            int c_0based = j - m2 + 1;

            if (c_0based < 0 || c_0based >= match_length.size()) {
                continue;
            }

            int k_representative = matched_row_indices[0];

            int L_val = match_length[c_0based];

            while (L_val > 0 && pattern_rows_global[k_representative] != pattern_rows_global[L_val]) {
                L_val = next_row[L_val - 1];
            }
            if (pattern_rows_global[k_representative] == pattern_rows_global[L_val]) {
                L_val++;
            }

            current_row_match_length[c_0based] = L_val;

            if (current_row_match_length[c_0based] == m1) {
                results.push_back({i - m1 + 1, c_0based + 1});
                current_row_match_length[c_0based] = next_row[m1 - 1];
            }
        }

        match_length = current_row_match_length;
    }

    return results;
}

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
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
        input_file.close();
        return 1;
    }

    int n1, n2, m1, m2;
    input_file >> m1 >> m2 >> n1 >> n2;

    std::vector<std::string> P(m1);
    for (int i = 0; i < m1; ++i) {
        input_file >> P[i];
    }

    std::vector<std::string> T(n1);
    for (int i = 0; i < n1; ++i) {
        input_file >> T[i];
    }

    input_file.close();

    std::vector<std::pair<int, int>> occurrences = PATTERN_2D_KMP_AC(T, P, n1, n2, m1, m2);

    output_file << occurrences.size() << "\n";

    for (const auto& p : occurrences) {
        output_file << p.first + 1 << " " << p.second << "\n";
    }

    output_file.close();

    return 0;
}
