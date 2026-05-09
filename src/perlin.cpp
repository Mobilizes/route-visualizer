#include "perlin.hpp"

#include <algorithm>
#include <stdexcept>

Perlin::Perlin(size_t width, size_t height, double scale)
: width(width), height(height), scale(scale)
{
  res.clear();
}

void Perlin::generate()
{
  std::mt19937 gen(std::random_device{}());

  res.clear();
  res.assign(width * height, 0);

  std::iota(perm, perm + 256, 0);
  std::shuffle(perm, perm + 256, gen);
  for (int i = 0; i < 256; ++i) perm[i + 256] = perm[i];

  for (size_t i = 0; i < width; ++i) {
    for (size_t j = 0; j < height; ++j) {
      res[i * height + j] =
        (perlin_noise(i / scale, j / scale) + 1.0) / 2.0;  // 0.0 <= res[i][j] <= 1.0
    }
  }
}

double Perlin::get(size_t i, size_t j)
{
  if (res.empty()) throw std::runtime_error("Map: not generated!");
  if (i >= width || j >= height || (i * height + j) >= (width * height))
    throw std::runtime_error("Map: Out of bounds index!");
  return res[i * height + j];
}

double Perlin::perlin_noise(double x, double y)
{
  int x_floor = std::floor(x);
  int y_floor = std::floor(y);

  double x_v = x - x_floor;
  double y_v = y - y_floor;

  x_floor &= 255;
  y_floor &= 255;

  std::pair<double, double> tr(x_v - 1.0, y_v - 1.0);
  std::pair<double, double> tl(x_v, y_v - 1.0);
  std::pair<double, double> br(x_v - 1.0, y_v);
  std::pair<double, double> bl(x_v, y_v);

  int tr_v = perm[perm[x_floor + 1] + y_floor + 1];
  int tl_v = perm[perm[x_floor] + y_floor + 1];
  int br_v = perm[perm[x_floor + 1] + y_floor];
  int bl_v = perm[perm[x_floor] + y_floor];

  double dot_tr = dot(tr, const_vector(tr_v));
  double dot_tl = dot(tl, const_vector(tl_v));
  double dot_br = dot(br, const_vector(br_v));
  double dot_bl = dot(bl, const_vector(bl_v));

  double u = fade(x_v);
  double v = fade(y_v);

  return lerp(u, lerp(v, dot_bl, dot_tl), lerp(v, dot_br, dot_tr));
}

std::pair<double, double> Perlin::const_vector(int v)
{
  v %= 4;

  switch (v) {
    case 0:
      return {1.0, 1.0};

    case 1:
      return {-1.0, 1.0};

    case 2:
      return {-1.0, -1.0};

    default:
      return {1.0, -1.0};
  }
}

double Perlin::dot(std::pair<double, double> vec1, std::pair<double, double> vec2)
{
  return vec1.first * vec2.first + vec1.second * vec2.second;
}

double Perlin::fade(double t) { return ((6 * t - 15) * t + 10) * t * t * t; }

double Perlin::lerp(double t, double a, double b) { return a + t * (b - a); }
