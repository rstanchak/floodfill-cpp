#ifndef ASTAR_HPP
#define ASTAR_HPP

#include "Graph.hpp"

namespace AStar {

/**
 * A* heuristic function for estimating cost from start to end
 *
 * A necessary property of the A* heuristic function is that it is
 * *admissible*.  I.e. the heuristic shall not overestimate the cost
 */
class Heuristic {
  public:
    // return estimated cost from start to end
    virtual int operator()(int start, int end) const = 0;
};

/**
 * Use A* search to find the lowest cost path between start and end 
 *
 * @param graph the graph to search
 * @param h the A* heuristic function to use
 * @param start the start node id in the graph
 * @param end the end node id in the graph
 * @return a vector of node ids leading from start to end
 */
std::vector<int> search(const Graph & graph,
    const Heuristic & h,
    int start,
    int end);

} // namespace AStar
#endif // ASTAR_HPP
