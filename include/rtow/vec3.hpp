#pragma once
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

template <typename T = double>
requires IsFloating<T>
class Vec3 {
public:
  static constexpr T NaN = std::numeric_limits<T>::quiet_NaN();

  Vec3(const T constant = T(0.))
      : data_{constant, constant, constant} {}

  Vec3(const T x, const T y, const T z) {
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
  }

  T x() const { return data_[0]; }
  T y() const { return data_[1]; }
  T z() const { return data_[2]; }
  const T* data() const { return data_; }
  T* data() { return data_; }

  T operator[](const size_t i) const { return data_[i]; }
  T& operator[](const size_t i) { return data_[i]; }

  Vec3<T> operator-() const {
    Vec3<T> v(-this->x(), -this->y(), -this->z());
    return v;
  }

  template <typename T2>
  Vec3<T>& operator+=(const Vec3<T2>& b) {
    this->x() = this->x() + b.x();
    this->y() = this->y() + b.y();
    this->z() = this->z() + b.z();
    return *this;
  }

  template <typename T2>
  Vec3<T>& operator*=(const T2& c) {
    this->x() = this->x() * c;
    this->y() = this->y() * c;
    this->z() = this->z() * c;
    return *this;
  }

  template <typename T2>
  Vec3<T>& operator/=(const T2& c) {
    this->x() = this->x() / c;
    this->y() = this->y() / c;
    this->z() = this->z() / c;
    return *this;
  }

  T norm() const { return std::sqrt(data_[0] * data_[0] + data_[1] * data_[1] + data_[2] * data_[2]); }
  T norm_squared() const { return data_[0] * data_[0] + data_[1] * data_[1] + data_[2] * data_[2]; }

  static Vec3 Constant(const T& constant) { return Vec3(constant); }

  std::string Print(size_t precison = 4) const {
    std::stringstream ss;
    ss << std::setprecision(precison) << "[ " << std::to_string(data_[0]) << ", " << std::to_string(data_[1])
       << ", " << std::to_string(data_[2]) << " ]";
    return ss.str();
  }

private:
  T data_[3] = {NaN, NaN, NaN};
};

}  // namespace rtow
