#ifndef ALGO_HPP
#define ALGO_HPP

#include "map.hpp"

#include <optional>
#include <utility>
#include <vector>

// Algo's lifetime must end at least at the same time as Map
class Algo
{
public:
  static std::optional<std::vector<std::pair<unsigned, unsigned>>> get_shortest_path(
    Map & map, unsigned src_i, unsigned src_j, unsigned dest_i, unsigned dest_j);
};

#endif  // ALGO_HPP
