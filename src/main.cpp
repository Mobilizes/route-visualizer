#include "algo.hpp"
#include "map.hpp"
#include "perlin.hpp"

#include <print>

#include <raylib.h>

int main()
{
  int perlin_width = 100;
  int perlin_height = 100;

  double perlin_scale = 5.0;
  Perlin perlin(perlin_width, perlin_height, perlin_scale);
  perlin.generate();

  int map_width = 10;
  int map_height = 10;

  unsigned int value_scale = 255;
  Map map(map_width, map_height, value_scale, perlin);
  map.generate();

  const std::vector<std::vector<unsigned int>> & vec_map = map.get();

  const int pixel_size = 32;
  const int subpixel_size = pixel_size - 8;
  const int x_offset = 120;
  const int y_offset = 60;
  int src_x = 0;
  int src_y = 0;
  int dest_x = 9;
  int dest_y = 0;

  InitWindow(800, 600, "Route Visualization");

  std::optional<std::vector<std::pair<unsigned, unsigned>>> path =
    Algo::get_shortest_path(map, src_x, src_y, dest_x, dest_y);
  while (!path.has_value()) {
    map.generate();
    path = Algo::get_shortest_path(map, src_x, src_y, dest_x, dest_y);
  }
  for (std::pair<unsigned, unsigned> i : path.value()) {
    std::println("{0} {1}", i.first, i.second);
  }

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);

    for (int i = 0; i < map_width; ++i) {
      for (int j = 0; j < map_height; ++j) {
        unsigned char v = vec_map[i][j];
        Color c = {v, v, v, 255};
        if (vec_map[i][j] == Map::NONROAD_TILE) c = {0, 0, 255, 255};

        DrawRectangle(
          x_offset + i * pixel_size, y_offset + j * pixel_size, pixel_size, pixel_size, c);
      }
    }

    int diff_size = pixel_size - subpixel_size;

    if (path.has_value()) {
      for (int i = 0; i < path.value().size(); ++i) {
        std::pair<unsigned, unsigned> pos = path.value()[i];
        DrawText(std::to_string(i + 1).c_str(), x_offset + diff_size / 2 + pos.first * pixel_size,
          y_offset + diff_size / 2 + pos.second * pixel_size, subpixel_size, {0, 0, 255, 255});
      }
    }

    EndDrawing();
  }

  CloseWindow();
}
