#pragma once

#include <stdint.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>

using Node = uint32_t;
using EdgeIndex = uint32_t;
using Edge = std::vector<Node>;
using NumNodes = uint32_t;
using NumEdges = uint32_t;

struct FibonacciHash {
    size_t operator()(uint32_t x) const {
        return static_cast<size_t>(x) * 2654435761u;
    }
};

using FastSet = std::unordered_set<Node, FibonacciHash>;
using FastMap = std::unordered_map<Node, size_t, FibonacciHash>;