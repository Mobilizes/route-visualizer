#include <algo.hpp>

#include <algorithm>
#include <array>
#include <climits>
#include <queue>
#include <vector>

const std::array<int, 4> dir_i = {1, 0, -1, 0};
const std::array<int, 4> dir_j = {0, 1, 0, -1};

std::optional<std::vector<std::pair<unsigned, unsigned>>> Algo::get_shortest_path(
  Map & map, unsigned src_i, unsigned src_j, unsigned dest_i, unsigned dest_j)
{
  if (src_i >= map.width || src_j >= map.height) return std::nullopt;

  std::priority_queue<std::vector<long long>, std::vector<std::vector<long long>>,
    std::greater<std::vector<long long>>>
    pq;
  std::vector<std::vector<bool>> visited(map.width, std::vector<bool>(map.height, false));
  std::vector<std::vector<std::pair<long long, unsigned>>> mp(
    map.width, std::vector<std::pair<long long, unsigned>>(map.height, {LLONG_MAX, 5}));
  const auto & map_ref = map.get();

  pq.push({0, src_i, src_j});
  visited[src_i][src_j] = true;

  // too lazy to create enums, 4 is source, 5 is unvisited, 0 1 2 3 is dir indexes
  mp[src_i][src_j] = {0, 4};

  while (!pq.empty()) {
    auto tp = pq.top();
    pq.pop();

    long long v = tp[0];
    int i = tp[1];
    int j = tp[2];

    if (i < 0 || j < 0 || i >= map.width || j >= map.height) continue;
    visited[i][j] = true;
    if (map_ref[i][j] == Map::NONROAD_TILE) continue;
    if (i == dest_i && j == dest_j) break;

    for (int k = 0; k < 4; ++k) {
      int a = i + dir_i[k];
      int b = j + dir_j[k];
      if (a < 0 || b < 0 || a > map.width - 1 || b > map.height - 1) continue;
      if (visited[a][b]) continue;
      if (map_ref[a][b] == Map::NONROAD_TILE) continue;

      if (v + map_ref[a][b] < mp[a][b].first) {
        mp[a][b] = {v + map_ref[a][b], k};
        pq.push({mp[a][b].first, a, b});
      }
    }
  }

  std::vector<std::pair<unsigned, unsigned>> path;
  int i = (int)dest_i;
  int j = (int)dest_j;

  if (mp[i][j].second == 5 || !visited[i][j]) return std::nullopt;

  while ((unsigned)i != src_i || (unsigned)j != src_j) {
    path.push_back({(unsigned)i, (unsigned)j});
    int k = (mp[i][j].second + 2) % 4;
    i += dir_i[k];
    j += dir_j[k];
  }

  path.push_back({(unsigned)i, (unsigned)j});
  std::reverse(path.begin(), path.end());

  return path;
}
