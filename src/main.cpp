#include "algo.hpp"
#include "map.hpp"
#include "perlin.hpp"

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

const int perlin_width = 100;
const int perlin_height = 100;
const double perlin_scale = 1.5;

int render_size = 3;
int subpixel_size = render_size * 2;
int pixel_size = subpixel_size * 2;
int diff_size = pixel_size - subpixel_size;

const int x_offset = 60;
const int y_offset = 60;

unsigned src_i = UINT32_MAX;
unsigned src_j = 0;
unsigned dest_i = UINT32_MAX;
unsigned dest_j = 0;

Perlin perlin(perlin_width, perlin_height, perlin_scale);
Map map(400 / pixel_size, 400 / pixel_size, 255, perlin);

std::string error_message = "";

float raw_render_size = render_size;
float raw_chance = map.chance;
float raw_perlin_scale = perlin.scale;
bool toggle_edit = false;

std::optional<std::vector<std::pair<unsigned, unsigned>>> path = std::nullopt;

const int lineThick = 4;

void UpdateDrawFrame(void);

int main()
{
  perlin.generate();
  map.generate();

  InitWindow(800, 600, "Route Visualization");

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
  SetTargetFPS(60);  // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose())  // Detect window close button or ESC key
  {
    UpdateDrawFrame();
  }
#endif

  CloseWindow();
}

void UpdateDrawFrame(void)
{
  BeginDrawing();
  ClearBackground(WHITE);

  DrawRectangleGradientH(x_offset, y_offset + 460, 400, 50, WHITE, BLACK);
  DrawRectangleLinesEx(Rectangle{x_offset - lineThick, y_offset + 460 - lineThick,
                         400 + 2 * lineThick, 50 + 2 * lineThick},
    lineThick, GRAY);
  DrawText("Weight representation of each tile:", x_offset, 475, 18, BLACK);
  DrawText("Low", x_offset, y_offset + 440, 14, BLACK);
  DrawText("High", x_offset + 375, y_offset + 440, 14, BLACK);

  for (int i = 0; i < static_cast<int>(map.width); ++i) {
    for (int j = 0; j < static_cast<int>(map.height); ++j) {
      unsigned char v = 255 - map.get()[i][j];
      Color c = {v, v, v, 255};
      if (map.get()[i][j] == Map::NONROAD_TILE) c = BLUE;

      DrawRectangle(
        x_offset + i * pixel_size, y_offset + j * pixel_size, pixel_size, pixel_size, c);
    }
  }

  DrawRectangleLinesEx(Rectangle{x_offset - lineThick, y_offset - lineThick,
                         static_cast<float>(map.width * pixel_size) + 2 * lineThick,
                         static_cast<float>(map.height * pixel_size) + 2 * lineThick},
    lineThick, Color{0, 0, 255, 255});

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

  bool reset = false;

  int mouse_x = GetMouseX();
  int mouse_y = GetMouseY();

  bool in_map =
    mouse_x >= x_offset && mouse_x < static_cast<int>(map.width) * pixel_size + x_offset &&
    mouse_y >= y_offset && mouse_y < static_cast<int>(map.height) * pixel_size + y_offset;

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    error_message = "";

    if (toggle_edit) {
    } else if (!in_map) {
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
        error_message = "Path not found!";
      }

      if (map.get()[dest_i][dest_j] == Map::NONROAD_TILE) {
        reset = true;
        error_message = "Please pick a weighted tile from the map.";
      }
    } else {
      reset = true;
    }
  }

  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
  } else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    if (toggle_edit) {
      reset = true;

      if (!in_map) {
      } else {
        unsigned i = (mouse_x - x_offset) / pixel_size;
        unsigned j = (mouse_y - y_offset) / pixel_size;

        map.get()[i][j] = 255;
      }
    }
  } else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
    if (toggle_edit) {
      reset = true;

      if (!in_map) {
      } else {
        unsigned i = (mouse_x - x_offset) / pixel_size;
        unsigned j = (mouse_y - y_offset) / pixel_size;

        if (map.get()[i][j] != Map::NONROAD_TILE) {
          map.get()[i][j] = Map::NONROAD_TILE;
        }
      }
    }
  }

  DrawText("Render size", x_offset + 440, y_offset, 16, BLACK);
  GuiSliderBar(
    Rectangle{x_offset + 440, y_offset + 20, 100, 50}, "1", "5", &raw_render_size, 1.0f, 5.0f);
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
    reset = true;
  }

  DrawText("Chance for initial points to grow", x_offset + 440, y_offset + 100, 16, BLACK);
  GuiSliderBar(
    Rectangle{x_offset + 440, y_offset + 120, 100, 50}, "0.0", "1.0", &raw_chance, 0.0f, 1.0f);
  if (std::fabs(raw_chance - map.chance) > 0.0001) {
    map.chance = raw_chance;

    map.generate();
    reset = true;
  }

  DrawText("Perlin Scale", x_offset + 440, y_offset + 200, 16, BLACK);
  GuiSliderBar(Rectangle{x_offset + 440, y_offset + 220, 100, 50}, "1.0", "10.0", &raw_perlin_scale,
    1.0f, 10.0f);
  if (std::fabs(raw_perlin_scale - perlin.scale) > 0.0001) {
    perlin.scale = raw_perlin_scale;

    perlin.generate();
    map.update_weights();

    if (path.has_value()) {
      path = Algo::get_shortest_path(map, src_i, src_j, dest_i, dest_j);
      reset = false;
    } else {
      reset = true;
    }
  }

  if (GuiButton(Rectangle{x_offset + 440, y_offset + 320, 100, 50}, "Regenerate Map")) {
    map.generate();

    reset = true;
  }

  if (GuiButton(Rectangle{x_offset + 590, y_offset + 320, 100, 50}, "Regenerate Weight")) {
    perlin.generate();
    map.update_weights();

    if (path.has_value()) {
      path = Algo::get_shortest_path(map, src_i, src_j, dest_i, dest_j);
      reset = false;
    } else {
      reset = true;
    }
  }

  if (GuiButton(Rectangle{x_offset + 440, y_offset + 420, 100, 50}, "Clear Map")) {
    map.clear();

    reset = true;
  }

  GuiCheckBox(Rectangle{x_offset + 590, y_offset + 420, 50, 50}, "Edit", &toggle_edit);

  if (reset) {
    src_i = UINT32_MAX;
    dest_i = UINT32_MAX;
    path = std::nullopt;
  }

  DrawText(error_message.c_str(), x_offset + 440, y_offset + 500, 14, RED);

  EndDrawing();
}
