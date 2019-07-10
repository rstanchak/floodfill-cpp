/*
 * Copyright Roman Stanchak 2019
 */
#include <vector>
#include <iostream>

#include "Graph.hpp"
#include "Bitmap.hpp"
#include "GraphTraversal.hpp"

int bitmap_xy_to_id(const Bitmap & bmp, int x, int y) {
  return y * bmp.width() + x;
}

void bitmap_id_to_xy(const Bitmap & bmp, int id, int & retarg_x, int & retarg_y) {
    retarg_x = id % bmp.width();
    retarg_y = id / bmp.width();
}

class ColorBitmapPixelOnVisit : public GraphTraversal::VisitCallback {
  protected:
    Bitmap & bmp_;
    char color_;

  public:
    ColorBitmapPixelOnVisit(Bitmap & bitmap, char color) : bmp_{bitmap}, color_{color} {}
    virtual void operator ()(int node_id) const {
      int x,y;
      bitmap_id_to_xy(bmp_, node_id, x, y);
      bmp_.set(x, y, color_);
    }
};

/** helper function to build a graph
 * from the bmp
 */
Graph buildGraphFromBitmap(const Bitmap & bmp) {
  Graph graph(bmp.height() * bmp.width());
  for (int i=0; i < bmp.height(); ++i) {
    for (int j=0; j < bmp.width(); ++j) {
      int id = bitmap_xy_to_id(bmp, j, i);
      int color = bmp.get(j, i);

      if (bmp.get(j-1, i) == color) {
        graph.add_edge(id, bitmap_xy_to_id(bmp, j-1, i));
      }
      if (bmp.get(j+1, i) == color) {
        graph.add_edge(id, bitmap_xy_to_id(bmp, j+1, i));
      }
      if (bmp.get(j, i-1) == color) {
        graph.add_edge(id, bitmap_xy_to_id(bmp, j, i-1));
      }
      if (bmp.get(j, i+1) == color) {
        graph.add_edge(id, bitmap_xy_to_id(bmp, j, i+1));
      }
    }
  }
  return graph;
}

int main() {
    // 1. read the input bmp
    Bitmap bmp;
    std::cin >> bmp;

    // 2. read the start (row, col)
    int row, col;
    std::cin >> row >> col;

    // 3. read the target color
    char target_color;
    std::cin >> target_color;

    // 4. convert it to a graph
    Graph graph = buildGraphFromBitmap(bmp);

    // 5. visit all nodes connected to start_id
    int start_id = bitmap_xy_to_id(bmp, col, row);
    GraphTraversal::bfs(graph, start_id, ColorBitmapPixelOnVisit(bmp, target_color));

    std::cout << bmp;
    return 0;
}
