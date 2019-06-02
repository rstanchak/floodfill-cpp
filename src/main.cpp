/*
 * Copyright Roman Stanchak 2019
 */
#include <vector>
#include <iostream>

#include "Graph.hpp"
#include "Map.hpp"
#include "AStar.hpp"

/**
 * A* heuristic.  The cost between any two nodes 
 * in a 2D grid is at least the L1 distance
 * i.e delta_x + delta_y
 */
class L1Heuristic : public AStar::Heuristic {
 public:
    int w_, h_;
    L1Heuristic(int w, int h): w_{w}, h_{h} {
    }
    int operator()(int start, int end) const {
      // decode id into x,y coordinates
      int startx, starty, endx, endy;
      startx = start % w_;
      starty = start / w_;
      endx = end % w_;
      endy = end / w_;
      // use L1 distance as heuristic
      return abs(endy-starty) + abs(endx-startx);
    }
};

/** helper function to build a graph
 * from the map
 */
Graph buildGraphFromMap(const Map & map) {
  Graph graph(map.height() * map.width());
  for (int i=0; i < map.height(); ++i) {
    for (int j=0; j < map.width(); ++j) {
      int id = map.xy_to_id(j, i);

      if (map.is_wall(j, i)) continue;

      if (!map.is_wall(j-1, i)) {
        graph.add_edge(id, map.xy_to_id(j-1, i));
      }
      if (!map.is_wall(j+1, i)) {
        graph.add_edge(id, map.xy_to_id(j+1, i));
      }
      if (!map.is_wall(j, i-1)) {
        graph.add_edge(id, map.xy_to_id(j, i-1));
      }
      if (!map.is_wall(j, i+1)) {
        graph.add_edge(id, map.xy_to_id(j, i+1));
      }
    }
  }
  return graph;
}

int main() {
    std::vector<std::string> rows;

    // 1. read the input map
    int n;
    std::cin >> n;

    for (int i=0; i < n; ++i) {
        std::string s;
        std::cin >> s;
        rows.push_back(s);
    }
    Map map{rows};

    // 2. convert it to a graph
    Graph graph = buildGraphFromMap(map);

    // 3. search for the shortest path between start and goal
    auto path = AStar::search(graph,
            L1Heuristic(map.width(), map.height()),
            map.start_id(),
            map.goal_id());

    // 4. map the nodes in the path to actions
    int prev_id = -1;
    for (int id : path) {
      if (prev_id >= 0) {
        std::cout << map.ids_to_action(prev_id, id);
      }
      prev_id = id;
    }
    std::cout << std::endl;
    return 0;
}
