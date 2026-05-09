#ifndef MAP_GEN_HPP
#define MAP_GEN_HPP

#include <ctime>
#include <random>
#include <utility>

class Perlin
{
private:
public:
  Perlin(unsigned int width, unsigned int height, double volatility);

  double scale;

  void generate();

  double get(size_t i, size_t j);

private:
  unsigned int width;
  unsigned int height;
  std::mt19937 gen;

  std::vector<double> res;

  double perlin_noise(double x, double y);
  std::pair<double, double> const_vector(int v);
  double dot(std::pair<double, double> vec1, std::pair<double, double> vec2);

  double fade(double t);
  double lerp(double t, double a, double b);

  int perm[512];
};

#endif  // MAP_GEN_HPP
