#ifndef GRAPH_HPP
#define GRAPH_HPP

/*
 * Copyright Roman Stanchak 2019
 */
#include <vector>

/**
 * Directed graph represented as an adjacency list
 */
class Graph {
 protected:
    std::vector<std::vector<int>> adj_;

 public:
    Graph(int node_count) {
      adj_.resize(node_count);
    }

    /**
     * get neighbors of node given id
     */
    const std::vector<int> & neighbors(int id) const { return adj_[id]; }

    /**
     * add a directed edge between node a and node b
     */
    void add_edge(int a, int b) { adj_[a].push_back(b); }
};

#endif // GRAPH_HPP

