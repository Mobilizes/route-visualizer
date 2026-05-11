#include "map.hpp"
#include "perlin.hpp"

#include <raylib.h>

int main()
{
  int perlin_width = 30;
  int perlin_height = 30;

  double perlin_scale = 5.0;
  Perlin perlin(perlin_width, perlin_height, perlin_scale);
  perlin.generate();

  int map_width = 20;
  int map_height = 20;

  unsigned int value_scale = 255;
  Map map(map_width, map_height, value_scale, perlin);
  map.generate();

  const std::vector<std::vector<unsigned int>> & vec_map = map.get();

  const int pixel_size = 16;

  InitWindow(800, 600, "Route Visualization");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);

    for (int i = 0; i < map_width; ++i) {
      for (int j = 0; j < map_height; ++j) {
        unsigned char v = vec_map[i][j];
        Color c = {v, v, v, 255};
        if (vec_map[i][j] == Map::NONROAD_TILE) c = {0, 0, 255, 255};

        DrawRectangle(i * pixel_size, j * pixel_size, pixel_size, pixel_size, c);
      }
    }

    EndDrawing();
  }

  CloseWindow();
}
