#include "map.hpp"

#include <stdexcept>

Map::Map(size_t width, size_t height, unsigned int scale, Perlin & perlin)
: width(width), height(height), scale(scale), perlin(perlin), count_non_empty(0)
{
  if (width == 0 || height == 0 || scale == 0) {
    throw std::invalid_argument("Map: Dimensions and scale must be greater than zero.");
  }

  if (scale == UINT32_MAX) {
    throw std::invalid_argument("Map: You can't give `scale` the same value as `UINT32_MAX`");
  }

  if (width > perlin.get_width()) {
    throw std::invalid_argument("Map: You can't give higher `width` than `perlin`'s `width`");
  }

  if (height > perlin.get_height()) {
    throw std::invalid_argument("Map: You can't give higher `height` than `perlin`'s `height`");
  }
}

void Map::generate()
{
  mt = std::mt19937(std::random_device{}());

  std::uniform_int_distribution<unsigned int> dist_i(0, perlin.get_width() - width);
  std::uniform_int_distribution<unsigned int> dist_j(0, perlin.get_height() - height);

  unsigned int i_offset = dist_i(mt);
  unsigned int j_offset = dist_j(mt);

  const std::vector<std::vector<double>> & noises = perlin.get();
  std::vector<std::vector<double>> sliced_perlin_noises;

  for (size_t i = 0; i < width; ++i) {
    const std::vector<double> & src = noises[i + i_offset];
    sliced_perlin_noises.push_back(
      std::vector<double>(src.begin() + j_offset, src.begin() + j_offset + height));
  }

  map.clear();
  map.assign(width, std::vector<unsigned int>(height, NONROAD_TILE));

  std::uniform_int_distribution<int> dist_pos_i(0, width - 1);
  std::uniform_int_distribution<int> dist_pos_j(0, height - 1);
  std::uniform_int_distribution<int> dist_dir(1, 4);

  do {
    int dir = dist_dir(mt);
    grow(dist_pos_i(mt), dist_pos_j(mt), dir);
  } while (count_non_empty < width * height / 3);

  unsigned int min_noise = UINT32_MAX;
  unsigned int max_noise = 0;

  for (size_t i = 0; i < width; ++i) {
    for (size_t j = 0; j < height; ++j) {
      if (map[i][j] == NONROAD_TILE) {
        continue;
      }
      map[i][j] = std::round(sliced_perlin_noises[i][j] * scale);
      min_noise = std::min(min_noise, map[i][j]);
      max_noise = std::max(max_noise, map[i][j]);
    }
  }

  for (size_t i = 0; i < width; ++i) {
    for (size_t j = 0; j < height; ++j) {
      if (map[i][j] == NONROAD_TILE) {
        continue;
      }
      map[i][j] = (map[i][j] - min_noise) * (255 / (max_noise - min_noise));
    }
  }

  // 700 - 1000
  // 0 - 255
  // 300 / 255 = 1.17647
  // 255 / 300 = 0.85
  // 750 -> 50 * 0.85 + 0 = 42.5
  // dest_min + (v - src_min) * (range_dest / range_src)
}

const std::vector<std::vector<unsigned int>> & Map::get()
{
  if (map.empty()) throw std::runtime_error("Perlin: called `get` before `generate()`");
  return map;
}
const size_t & Map::get_width() { return width; }
const size_t & Map::get_height() { return height; }

bool Map::grow(int i, int j, int from_dir)
{
  if (i < 0 || i >= width || j < 0 || j >= height) return false;

  int to_dir = ((from_dir - 1) + 2) % 4 + 1;

  int dist_to_bounds;
  switch (to_dir) {
    case DIR_LEFT: {
      dist_to_bounds = i;
      break;
    }

    case DIR_UP: {
      dist_to_bounds = j;
      break;
    }

    case DIR_RIGHT: {
      dist_to_bounds = width - i - 1;
      break;
    }

    case DIR_DOWN: {
      dist_to_bounds = height - j - 1;
      break;
    }
  }

  if (dist_to_bounds == 0) {
    return true;
  }

  std::uniform_int_distribution<int> dist_dist(1, dist_to_bounds);
  int dist = dist_dist(mt);

  const double CHANCE = 0.4;
  std::uniform_real_distribution<double> dist_chance(0, 1);

  switch (to_dir) {
    case DIR_LEFT: {
      for (int idx = i - 1; idx >= i - dist; --idx) {
        if (count_non_empty_neighboring_tile(idx, j, from_dir)) break;

        if (map[idx][j] == NONROAD_TILE) count_non_empty++;
        map[idx][j] = 0;
        if (dist_chance(mt) <= CHANCE) grow(idx, j, DIR_DOWN);
        if (dist_chance(mt) <= CHANCE) grow(idx, j, DIR_UP);
      }
      break;
    }

    case DIR_UP: {
      for (int idx = j - 1; idx >= j - dist; --idx) {
        if (count_non_empty_neighboring_tile(i, idx, from_dir)) break;

        if (map[idx][j] == NONROAD_TILE) count_non_empty++;
        map[i][idx] = 0;
        if (dist_chance(mt) <= CHANCE) grow(i, idx, DIR_LEFT);
        if (dist_chance(mt) <= CHANCE) grow(i, idx, DIR_RIGHT);
      }
      break;
    }

    case DIR_RIGHT: {
      for (int idx = i + 1; idx <= i + dist; ++idx) {
        if (count_non_empty_neighboring_tile(idx, j, from_dir)) break;

        if (map[idx][j] == NONROAD_TILE) count_non_empty++;
        map[idx][j] = 0;
        if (dist_chance(mt) <= CHANCE) grow(idx, j, DIR_DOWN);
        if (dist_chance(mt) <= CHANCE) grow(idx, j, DIR_UP);
      }
      break;
    }

    case DIR_DOWN: {
      for (int idx = j + 1; idx <= j + dist; ++idx) {
        if (count_non_empty_neighboring_tile(i, idx, from_dir)) break;

        if (map[idx][j] == NONROAD_TILE) count_non_empty++;
        map[i][idx] = 0;
        if (dist_chance(mt) <= CHANCE) grow(i, idx, DIR_LEFT);
        if (dist_chance(mt) <= CHANCE) grow(i, idx, DIR_RIGHT);
      }
      break;
    }
  }

  return true;
}

int Map::count_non_empty_neighboring_tile(int i, int j, int from_dir)
{
  int to_dir = ((from_dir - 1) + 2) % 4 + 1;
  return (i - 1 >= 0 && !(from_dir & 1) && !(to_dir & 1) ? map[i - 1][j] != NONROAD_TILE : 0) +
         (j - 1 >= 0 && (from_dir & 1) && (to_dir & 1) ? map[i][j - 1] != NONROAD_TILE : 0) +
         (i + 1 < width && !(from_dir & 1) && !(to_dir & 1) ? map[i + 1][j] != NONROAD_TILE : 0) +
         (j + 1 < height && (from_dir & 1) && (to_dir & 1) ? map[i][j + 1] != NONROAD_TILE : 0);
}
