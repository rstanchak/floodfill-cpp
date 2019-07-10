#ifndef BITMAP_HPP
#define BITMAP_HPP

/*
 * Copyright Roman Stanchak 2019
 */
#include <vector>
#include <iostream>

/*
 * 2D Bitmap represented in ascii
 */
class Bitmap {
 protected:
    // rows of the map
    std::vector<std::string> rows_;

 public:

    Bitmap() {}

    friend std::istream &operator>>(std::istream &input, Bitmap & b ) { 
      int n;
      input >> n;

      for (int i=0; i < n; ++i) {
        std::string s;
        input >> s;
        b.rows_.push_back(s);
      }
      return input;
    }

    friend std::ostream &operator<<(std::ostream &output, Bitmap & b ) { 
      output << b.rows_.size() << std::endl;
      for (std::string s : b.rows_){
        output << s << std::endl;
      }
      return output;
    }

    /**
     */
    int get(int x, int y) const {
      if (x<0 || x >= rows_[0].size() || y<0 || y >= rows_.size()) return -1;
      return (int) rows_[y][x];
    }

    /**
     */
    void set(int x, int y, char value) {
      if (x<0 || x >= rows_[0].size() || y<0 || y >= rows_.size()) return;
      rows_[y][x] = value;
    }

    int width() const { return rows_.size() > 0 ? rows_[0].size() : 0; }
    int height() const { return rows_.size(); }

};

#endif // BITMAP_HPP

