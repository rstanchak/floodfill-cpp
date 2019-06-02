#ifndef MAP_HPP
#define MAP_HPP

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
    int start_;
    int goal_;

 public:

    /** input is a vector of strings containing the rows
     * of the map in ascii -- 
     * | is a wall
     * S is the start
     * G is the goal
     * anything else is empty space
     */
    explicit Map(std::vector<std::string> map) :
      grid_{map},
      w_{map[0].size()},
      h_{map.size()} {
        for(int i=0; i<h_; ++i){
          for(int j=0; j<w_; ++j){
            if (is_goal(j, i)) {
              goal_ = xy_to_id(j, i);
            }
            else if (is_start(j, i)) {
              start_ = xy_to_id(j, i);
            }
          }
        }
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
     * transform the point x, y to a coordinate id
     * 
     * invalid if x,y is outside the map
     */
    int xy_to_id(int x, int y) const {
      return w_*y + x;
    }

    /**
     * transform the coordinate id to the corresponding x,y point
     * 
     * valid iff 0 <= id < width*height
     */
    std::pair<int, int> id_to_xy(int id) const {
      return {id % width(), id / width()};
    }

    /**
     * return the action to get from the coordinate prev_id to id
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

    const int & start_id() const { return start_; }
    const int & goal_id() const { return goal_; }
};

#endif // MAP_HPP

