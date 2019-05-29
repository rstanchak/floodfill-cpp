/*
 * Copyright Roman Stanchak 2019
 */
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue> /* priority_queue */
#include <algorithm> /* reverse */

/*
 * Helper class to work with 2D map represented in ascii
 */
class Map {
 protected:
    // rows of the map
    std::vector<std::string> grid_;
    // map dimensions
    size_t w_, h_;

 public:
    explicit Map(std::vector<std::string> map) :
      grid_{map},
      w_{map[0].size()},
      h_{map.size()} {
    }

    /**
     * determine if the point (x, y) is a wall or not
     *
     * points outside the map are considered wall
     */
    bool is_wall(int x, int y) const {
      if (x<0 || x >= w_ || y<0 || y >= h_) return true;
      return grid_[y][x] == '|';
    }

    /**
     * determine if the point (x, y) is the goal 
     */
    bool is_goal(int x, int y) const {
      if (x<0 || x >= w_ || y<0 || y >= h_) return false;
      return grid_[y][x] == 'F';
    }

    /**
     * determine if the point (x, y) is the start 
     */
    bool is_start(int x, int y) const {
      if (x<0 || x >= w_ || y<0 || y >= h_) return false;
      return grid_[y][x] == 'S';
    }

    /**
     * map width accessor
     */
    const size_t & width() const { return w_; }

    /**
     * map height accessor
     */
    const size_t & height() const { return h_; }

    /**
     * transform the point x, y to a graphnode id
     * 
     * invalid if x,y is outside the map
     */
    int xy_to_id(int x, int y) const {
      return w_*y + x;
    }

    /**
     * transform the graph node id to the corresponding x,y point
     * 
     * valid iff 0 <= id < width*height
     */
    std::pair<int, int> id_to_xy(int id) const {
      return {id % width(), id / width()};
    }

    /**
     * return the action to get from the node prev_id to id
     * 
     * assumes prev_id and id are adjacent
     */
    char ids_to_action(int prev_id, int id) {
      auto prev_pt = this->id_to_xy(prev_id);
      auto pt = this->id_to_xy(id);

      if (prev_pt.first < pt.first) return 'R';
      if (prev_pt.first > pt.first) return 'L';
      if (prev_pt.second < pt.second) return 'D';
      if (prev_pt.second > pt.second) return 'U';
      return '.';
    }
};



class Graph {
    std::vector<std::vector<int>> adj;
    int start_, goal_;

 public:
    Graph() : start_{-1}, goal_{-1} {}
    void buildFromMap(const Map & map) {
      adj.resize(map.height() * map.width());
      for (int i=0; i < map.height(); ++i) {
        for (int j=0; j < map.width(); ++j) {
          int id = map.xy_to_id(j, i);

          if (map.is_wall(j, i)) continue;

          if (map.is_start(j, i)) start_ = id;
          if (map.is_goal(j, i)) goal_ = id;

          if (!map.is_wall(j-1, i)) {
            adj[id].push_back(map.xy_to_id(j-1, i));
          }
          if (!map.is_wall(j+1, i)) {
            adj[id].push_back(map.xy_to_id(j+1, i));
          }
          if (!map.is_wall(j, i-1)) {
            adj[id].push_back(map.xy_to_id(j, i-1));
          }
          if (!map.is_wall(j, i+1)) {
            adj[id].push_back(map.xy_to_id(j, i+1));
          }
        }
      }
    }
    const int & start() const { return start_; }
    const int & goal() const { return goal_; }
    const std::vector<int> & neightbors(int id) const { return adj[id]; }
};

class Heuristic {
 public:
    virtual int operator()(int start, int end) const = 0;
};

class L1Heuristic : public Heuristic {
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


std::vector<int> reconstruct_path(std::unordered_map<int, int> prev, int id) {
  std::vector<int> path;
  while (prev.find(id) != std::end(prev)) {
    path.push_back(id);
    id = prev[id];
  }
  path.push_back(id);
  std::reverse(std::begin(path), std::end(path));
  return path;
}


std::vector<int> a_star(const Graph & graph,
                        const Heuristic & h,
                        int start,
                        int end) {
  std::unordered_set<int> open_set;    // nodes being evaluated
  std::unordered_set<int> closed_set;  // nodes already evaluated
  std::unordered_map<int, int> f;  // est. cost from start to end through node
  std::unordered_map<int, int> g;  // cost from start to node
  std::unordered_map<int, int> prev;   // most efficient previous step to node

  auto compare = [&f](int a, int b){
    if (f.find(b) == std::end(f)) {
      return true;   // if b is undefined, then a is least
    }
    if (f.find(a) == std::end(f)) {
      return false;  // if a is undefined, then b is least
    }
    return f[a] <= f[b];
  };

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

    for (int neighbor_id : graph.neightbors(current)) {
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

  // no path???
  return {};
}


int main() {
    std::vector<std::string> rows;
    int n;
    std::cin >> n;

    for (int i=0; i < n; ++i) {
        std::string s;
        std::cin >> s;
        rows.push_back(s);
    }
    Map map{rows};

    Graph graph;
    graph.buildFromMap(map);

    auto path = a_star(graph,
            L1Heuristic(map.width(), map.height()),
            graph.start(),
            graph.goal());

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
