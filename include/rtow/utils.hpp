#pragma once

#include <random>

namespace rtow {
template <typename T>
inline T random(const T min = T(0), const T max = T(1), T seed = T(42)) {
  static std::uniform_real_distribution<T> distribution(min, max);
  static std::mt19937 generator(seed);
  return distribution(generator);
}

}  // namespace rtow