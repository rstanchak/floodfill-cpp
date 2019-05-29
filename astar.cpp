/*
 * Copyright Roman Stanchak 2019
 */
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue> /* priority_queue */
#include <algorithm> /* reverse */

class Map {
 public:
    std::vector<std::string> grid_;
    size_t w_, h_;
    explicit Map(std::vector<std::string> map) :
      grid_{map},
      w_{map[0].size()},
      h_{map.size()} {
      }
    bool is_wall(int x, int y) const {
      if (x<0 || x >= w_ || y<0 || y >= h_) return true;
      return grid_[y][x] == '|';
    }
    bool is_end(int x, int y) const {
      if (x<0 || x >= w_ || y<0 || y >= h_) return false;
      return grid_[y][x] == 'F';
    }
    bool is_start(int x, int y) const {
      if (x<0 || x >= w_ || y<0 || y >= h_) return false;
      return grid_[y][x] == 'S';
    }
};

int xy_to_id(int w, int h, int x, int y) {
  return w*y + x;
}

std::pair<int, int> id_to_xy(int w, int h, int id) {
  return {id % w, id / w};
}

class Graph {
 public:
    std::vector<std::vector<int>> adj;
    int start_, end_;

    void buildFromMap(const Map & map) {
      adj.resize(map.h_ * map.w_);
      for (int i=0; i < map.h_; ++i) {
        for (int j=0; j < map.w_; ++j) {
          int id = xy_to_id(map.w_, map.h_, j, i);

          if (map.is_wall(j, i)) continue;

          if (map.is_start(j, i)) start_ = id;
          if (map.is_end(j, i)) end_ = id;

          if (!map.is_wall(j-1, i)) {
            adj[id].push_back(xy_to_id(map.w_, map.h_, j-1, i));
          }
          if (!map.is_wall(j+1, i)) {
            adj[id].push_back(xy_to_id(map.w_, map.h_, j+1, i));
          }
          if (!map.is_wall(j, i-1)) {
            adj[id].push_back(xy_to_id(map.w_, map.h_, j, i-1));
          }
          if (!map.is_wall(j, i+1)) {
            adj[id].push_back(xy_to_id(map.w_, map.h_, j, i+1));
          }
        }
      }
    }
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

    for (int neighbor_id : graph.adj[current]) {
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

std::vector<char> id_to_action(const std::vector<int> & path) {
    std::vector<char> actions;
    int prev_id = -1;
    for (int id : path) {
        if (prev_id >= 0) {
            int diff = id - prev_id;
            // nodes are enumerated left to right top to bottom
            // assuming valid steps...
            // diff of 1 is one step right
            if (diff == 1) actions.push_back('R');
            // diff of -1 is one step left
            else if (diff == -1) actions.push_back('L');
            // diff < 0 is step up
            else if (diff < 0) actions.push_back('U');
            // diff > 0 is step down
            else if (diff > 0) actions.push_back('D');
        }
        prev_id = id;
    }
    return actions;
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
            L1Heuristic(map.w_, map.h_),
            graph.start_,
            graph.end_);

    for (char c : id_to_action(path)) {
        std::cout << c;
    }
    std::cout << std::endl;
    return 0;
}
