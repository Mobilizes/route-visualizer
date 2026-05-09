#include "perlin.hpp"

#include <raylib.h>

int main()
{
  int width = 300;
  int height = 300;
  double scale = 10.0;

  Perlin map(width, height, scale);
  map.generate();

  InitWindow(800, 600, "Route Visualization");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);

    map.generate();

    int rect_width = std::max(1, 800 / width);
    int rect_height = std::max(1, 600 / height);
    for (int i = 0; i < width; ++i) {
      for (int j = 0; j < height; ++j) {
        unsigned char val = 255 * map.get()[i][j];
        DrawRectangle(i * rect_width, j * rect_height, rect_width,
          rect_height, {val, val, val, 255});
      }
    }

    EndDrawing();
  }

  CloseWindow();
}
