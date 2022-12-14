#pragma once
#include <array>
#include <cmath>
#include <concepts>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>

#include "rtow/utils.hpp"

namespace rtow {

template <typename T1>
concept IsFloating = std::is_floating_point_v<T1>;

template <size_t N>
concept IsVec3 = (N == 3U);

template <size_t N>
concept IsVec2 = (N == 2U);

template <typename T, size_t N>
requires IsFloating<T>
class Vec {
public:
  static constexpr T NaN = std::numeric_limits<T>::quiet_NaN();

  Vec(const T constant = T(NaN)) {
    for (T& el : data_) {
      el = constant;
    }
  }

  Vec(const std::array<T, N>& l) {
    for (size_t i = 0; i < N; ++i) {
      this->data_[i] = std::data(l)[i];
    }
  }

  Vec(const T x, const T y) requires IsVec2<N> {
    data_[0] = x;
    data_[1] = y;
  }

  Vec(const T x, const T y, const T z) requires IsVec3<N> {
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
  }

  T x() const requires IsVec3<N> || IsVec2<N> { return data_[0]; }
  T y() const requires IsVec3<N> || IsVec2<N> { return data_[1]; }
  T z() const requires IsVec3<N> { return data_[2]; }

  T& x() requires IsVec3<N> || IsVec2<N> { return data_[0]; }
  T& y() requires IsVec3<N> || IsVec2<N> { return data_[1]; }
  T& z() requires IsVec3<N> { return data_[2]; }

  const T* data() const { return data_.data(); }
  T* data() { return data_.data(); }

  const T& operator[](const size_t i) const { return data_[i]; }
  T& operator[](const size_t i) { return data_[i]; }

  Vec<T, N> operator-() const {
    Vec<T, N> v;
    for (size_t i = 0; i < N; ++i) {
      v[i] = -this->data_[i];
    }
    return v;
  }

  template <typename T2>
  Vec<T, N>& operator+=(const Vec<T2, N>& c) {
    for (size_t i = 0; i < N; ++i) {
      this->data_[i] += c[i];
    }
    return *this;
  }

  template <typename T2>
  Vec<T, N>& operator*=(const T2& c) {
    for (size_t i = 0; i < N; ++i) {
      this->data_[i] *= c;
    }
    return *this;
  }

  Vec<T, N>& operator*=(const Vec<T, N>& v) {
    for (size_t i = 0; i < N; ++i) {
      this->data_[i] *= v[i];
    }
    return *this;
  }

  template <typename T2>
  Vec<T, N>& operator/=(const T2& c) {
    for (size_t i = 0; i < N; ++i) {
      this->data_[i] /= c;
    }
    return *this;
  }

  T norm_squared() const {
    T sum = T(0);
    for (const T& el : this->data_) {
      sum += el * el;
    }
    return sum;
  }

  bool has_NaN() const {
    for (size_t i = 0; i < N; ++i) {
      if (std::isnan(data_[i])) {
        return true;
      }
    }

    return false;
  }

  T norm() const { return std::sqrt(norm_squared()); }

  static Vec<T, N> constant(const T& constant) { return Vec<T, N>(constant); }

  static Vec<T, N> random(const T min, const T max) {
    Vec<T, N> v;
    for (size_t i = 0; i < N; ++i) {
      v[i] = rtow::random(min, max);
    }

    return v;
  }

  std::string Print(size_t precison = 4) const {
    std::stringstream ss;
    ss << std::setprecision(precison) << "[ ";
    for (size_t i = 0; i < N - 1; ++i) {
      ss << std::to_string(data_[i]) << ", ";
    }
    ss << std::to_string(data_[N - 1]) << " ]";

    return ss.str();
  }

private:
  std::array<T, N> data_;
};

template <typename T>
using Vec3 = Vec<T, 3>;

template <typename T>
using Vec2 = Vec<T, 2>;

using Vec3d = Vec3<double>;
using Vec3f = Vec3<float>;
using Vec2d = Vec2<double>;
using Vec2f = Vec2<float>;

}  // namespace rtow
