#ifndef MAP_GEN_HPP
#define MAP_GEN_HPP

#include <ctime>
#include <random>
#include <utility>

class Perlin
{
private:
public:
  Perlin(size_t width, size_t height, double volatility);

  double scale;

  void generate();

  const std::vector<std::vector<double>> & get();
  const size_t & get_width();
  const size_t & get_height();

private:
  size_t width;
  size_t height;

  std::mt19937 mt;

  std::vector<std::vector<double>> noises;

  double perlin_noise(double x, double y);
  std::pair<double, double> const_vector(int v);
  double dot(std::pair<double, double> vec1, std::pair<double, double> vec2);

  double fade(double t);
  double lerp(double t, double a, double b);

  int perm[512];
};

#endif  // MAP_GEN_HPP
