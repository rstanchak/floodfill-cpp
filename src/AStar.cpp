#include <algorithm> // reverse
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include "AStar.hpp"

namespace AStar {

/**
 * helper function to reconstruct the path to get to node <id> given a map
 * of node a => b, where the best path to a is from b
 */
std::vector<int> reconstruct_path(std::unordered_map<int, int> prev, int id){
  std::vector<int> path;
  while (prev.find(id) != std::end(prev)) {
    path.push_back(id);
    id = prev[id];
  }
  path.push_back(id);
  std::reverse(std::begin(path), std::end(path));
  return path;
}

// algorithm courtesy of https://en.wikipedia.org/wiki/A*_search_algorithm
std::vector<int> search(const Graph & graph,
    const Heuristic & h,
    int start,
    int end) {
  std::unordered_set<int> open_set;    // nodes being evaluated
  std::unordered_set<int> closed_set;  // nodes already evaluated
  std::unordered_map<int, int> f;      // est. cost from start to end through node
  std::unordered_map<int, int> g;      // cost from start to node
  std::unordered_map<int, int> prev;   // most efficient previous step to node

  // comparison function to favor the path with the lowest estimated cost
  auto compare = [&f](int a, int b){
    if (f.find(b) == std::end(f)) {
      return true;   // if b is undefined, then a is least
    }
    if (f.find(a) == std::end(f)) {
      return false;  // if a is undefined, then b is least
    }
    return f[a] <= f[b];
  };

  // eval_queu contains the open_set sorted by
  // shortest estimated cost
  std::priority_queue<int, std::vector<int>, decltype(compare)>
    eval_queue(compare);
  g[start] = 0;  // cost of getting from start to start=0
  f[start] = h(start, end);

  eval_queue.push(start);
  open_set.insert(start);

  while (!eval_queue.empty()) {
    int current = eval_queue.top();
    eval_queue.pop();
    open_set.erase(current);

    if (current == end) {
      return reconstruct_path(prev, end);
    }

    closed_set.insert(current);

    for (int neighbor_id : graph.neighbors(current)) {
      if (closed_set.count(neighbor_id) > 0) {
        // skip already processed neighbors
        continue;
      }
      int cost = g[current] + 1;  // 1-step to neighbors
      if (open_set.count(neighbor_id) == 0) {
        // new node
        open_set.insert(neighbor_id);
        eval_queue.push(neighbor_id);
      }
      if (g.find(neighbor_id) == g.end() || cost < g[neighbor_id]) {
        // the path to neighbor is lowest cost so far
        // update path, costs
        prev[neighbor_id] = current;
        g[neighbor_id] = cost;
        f[neighbor_id] = g[neighbor_id] + h(neighbor_id, end);
      }
    }
  }

  // no path found to goal
  return {};
}

} // namespace AStar
