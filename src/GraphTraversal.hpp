#ifndef GRAPH_TRAVERSAL_HPP
#define GRAPH_TRAVERSAL_HPP

#include "Graph.hpp"

namespace GraphTraversal {

/**
 */
class VisitCallback {
  public:
    // called when the node is visited
    virtual void operator()(int node_id) const = 0;
};

/**
 * Use A* search to find the lowest cost path between start and end 
 *
 * @param graph the graph to traverse 
 * @param start the start node id in the graph
 * @param visit_callback a function to call everytime a new node is visited
 */
void bfs(const Graph & graph,
    int start_id,
    const VisitCallback & visit_callback);

} // namespace GraphTraversal
#endif // GRAPH_TRAVERSAL_HPP
