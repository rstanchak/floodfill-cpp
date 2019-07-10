#include <algorithm> // reverse
#include <unordered_set>
#include <unordered_map>
#include <list>
#include "GraphTraversal.hpp"

namespace GraphTraversal {

void bfs(const Graph & graph,
    int start_id,
    const VisitCallback & visit_callback) {
  std::list<int> visit_queue;    // queue of nodes to visit
  std::unordered_set<int> visited;  // nodes already visited 

  visit_queue.push_back(start_id);

  while (!visit_queue.empty()) {
    int current = visit_queue.front();
    visit_queue.pop_front();
    visit_callback(current);
    visited.insert(current);

    for (int neighbor_id : graph.neighbors(current)) {
      if (visited.count(neighbor_id) > 0) {
        // skip already visited neighbors
        continue;
      }
      visit_queue.push_back(neighbor_id);
    }
  }
}

} // namespace GraphTraversal
