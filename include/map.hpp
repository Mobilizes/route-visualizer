#ifndef MAP_HPP
#define MAP_HPP

#include "perlin.hpp"

#include <cstddef>
#include <vector>

// Map's lifetime must end at least at the same time as Perlin
class Map
{
public:
  enum
  {
    NONROAD_TILE = UINT32_MAX
  };

  Map(size_t width, size_t height, unsigned int scale, Perlin & perlin);

  void generate();

  const std::vector<std::vector<unsigned int>> & get();

  size_t width;
  size_t height;
  double chance;

private:
  enum
  {
    DIR_LEFT = 1,
    DIR_UP = 2,
    DIR_RIGHT = 3,
    DIR_DOWN = 4,
  };

  bool grow(int i, int j, int from_dir);

  int count_non_empty_neighboring_tile(int i, int j, int from_dir);

  unsigned int scale;

  std::mt19937 mt;

  Perlin & perlin;

  std::vector<std::vector<unsigned int>> map;
  int count_non_empty;
};

#endif  // MAP_HPP
