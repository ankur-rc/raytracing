#pragma once
#include <array>
#include <cmath>
#include <concepts>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>

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

  T norm() const { return std::sqrt(norm_squared()); }

  static Vec<T, N> Constant(const T& constant) { return Vec<T, N>(constant); }

  std::string Print(size_t precison = 4) const {
    std::stringstream ss;
    ss << std::setprecision(precison) << "[ ";
    for (size_t i = 0; i < N - 1; ++i) {
      ss << std::to_string(data_[i]) << ", ";
    }
    ss << data_[N - 1] << " ]";

    return ss.str();
  }

private:
  std::array<T, N> data_;
};

}  // namespace rtow
