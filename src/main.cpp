#include "algo.hpp"
#include "map.hpp"
#include "perlin.hpp"

#include <raylib.h>
#include <print>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main()
{
  int perlin_width = 100;
  int perlin_height = 100;

  double perlin_scale = 1.5;
  Perlin perlin(perlin_width, perlin_height, perlin_scale);
  perlin.generate();

  int render_size = 3;
  int subpixel_size = render_size * 2;
  int pixel_size = subpixel_size * 2;
  int diff_size = pixel_size - subpixel_size;

  int map_width = 400 / pixel_size;
  int map_height = 400 / pixel_size;

  unsigned int value_scale = 255;
  Map map(map_width, map_height, value_scale, perlin);
  map.generate();

  const int x_offset = 60;
  const int y_offset = 60;

  unsigned src_i = UINT32_MAX;
  unsigned src_j = 0;
  unsigned dest_i = UINT32_MAX;
  unsigned dest_j = 0;

  InitWindow(800, 600, "Route Visualization");

  float raw_render_size = render_size;

  std::optional<std::vector<std::pair<unsigned, unsigned>>> path = std::nullopt;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);

    for (int i = 0; i < static_cast<int>(map.width); ++i) {
      for (int j = 0; j < static_cast<int>(map.height); ++j) {
        unsigned char v = map.get()[i][j];
        Color c = {v, v, v, 255};
        if (map.get()[i][j] == Map::NONROAD_TILE) c = {0, 0, 255, 255};

        DrawRectangle(
          x_offset + i * pixel_size, y_offset + j * pixel_size, pixel_size, pixel_size, c);
      }
    }

    if (src_i != UINT32_MAX) {
      DrawRectangle(x_offset + diff_size / 2 + src_i * pixel_size,
        y_offset + diff_size / 2 + src_j * pixel_size, subpixel_size, subpixel_size, RED);
    }

    if (path.has_value()) {
      for (int i = 0; i < path->size(); ++i) {
        unsigned char v = std::min(255, static_cast<int>(200 + (255 - 200) / (path->size() - i)));
        Color c = {0, v, 0, 255};

        std::pair<unsigned, unsigned> pos = path.value()[i];
        DrawRectangle(x_offset + diff_size / 2 + pos.first * pixel_size,
          y_offset + diff_size / 2 + pos.second * pixel_size, subpixel_size, subpixel_size, c);
      }
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      int mouse_x = GetMouseX();
      int mouse_y = GetMouseY();

      bool reset = false;

      if (mouse_x < x_offset || mouse_x > static_cast<int>(map.width) * pixel_size + x_offset ||
          mouse_y < y_offset || mouse_y > static_cast<int>(map.height) * pixel_size + y_offset) {
        reset = true;
      } else if (src_i == UINT32_MAX) {
        src_i = (mouse_x - x_offset) / pixel_size;
        src_j = (mouse_y - y_offset) / pixel_size;

        if (map.get()[src_i][src_j] == Map::NONROAD_TILE) {
          reset = true;
        }

      } else if (dest_i == UINT32_MAX) {
        dest_i = (mouse_x - x_offset) / pixel_size;
        dest_j = (mouse_y - y_offset) / pixel_size;

        path = Algo::get_shortest_path(map, src_i, src_j, dest_i, dest_j);
        if (!path.has_value()) {
          reset = true;
        }
      } else {
        reset = true;
      }

      if (reset) {
        src_i = UINT32_MAX;
        dest_i = UINT32_MAX;
        path = std::nullopt;
      }
    }

    GuiSliderBar(Rectangle(500, 150, 100, 50), "min", "max", &raw_render_size, 1.0f, 5.0f);
    int next_render_size = static_cast<int>(std::round(raw_render_size));

    if (next_render_size != render_size) {
      render_size = next_render_size;
      raw_render_size = static_cast<float>(render_size);

      subpixel_size = render_size * 2;
      pixel_size = subpixel_size * 2;
      diff_size = pixel_size - subpixel_size;

      map.width = 400 / pixel_size;
      map.height = 400 / pixel_size;
      map.generate();

      src_i = UINT32_MAX;
      src_j = 0;
      dest_i = UINT32_MAX;
      dest_j = 0;
      path = std::nullopt;
    }

    if (GuiButton(Rectangle(510, 300, 80, 50), "Regenerate")) {
      map.generate();

      src_i = UINT32_MAX;
      src_j = 0;
      dest_i = UINT32_MAX;
      dest_j = 0;
      path = std::nullopt;
    }

    EndDrawing();
  }

  CloseWindow();
}
