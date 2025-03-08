#pragma once

#include <vector>
#include <unordered_set>

// For some unknown reason neither tuple nor pair have built-in hash functions, so yo need to define one
// yourself.
template <typename... T>
struct hash_tuple {
    std::size_t operator ()(const std::tuple<T...>& t) const {
        return std::apply([](const auto&... args) {
            return (std::hash<std::decay_t<decltype(args)>>{}(args) ^ ...);
        }, t);
    }
};

template <typename T1, typename T2>
struct hash_pair : public hash_tuple<T1, T2> {};

using index_size = size_t;

using SolutionSet = std::unordered_set<std::pair<index_size, index_size>, hash_pair<index_size, index_size>>;

template <typename T>
using Matrix = std::vector<std::vector<T>>;
