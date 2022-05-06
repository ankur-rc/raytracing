#pragma once

#include <random>

namespace rtow {
template <typename T>
inline T random(const T min = T(0), const T max = T(1)) {
  static std::uniform_real_distribution<T> distribution(min, max);
  static std::mt19937 generator;
  return distribution(generator);
}

}  // namespace rtow