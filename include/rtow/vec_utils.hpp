#pragma once

#include "rtow/utils.hpp"
#include "rtow/vec.hpp"
namespace rtow {
template <typename T>
using Vec3 = Vec<T, 3>;

template <typename T>
using Vec2 = Vec<T, 2>;

using Vec3d = Vec3<double>;
using Vec3f = Vec3<float>;
using Vec2d = Vec2<double>;
using Vec2f = Vec2<float>;

template <typename T1, typename T2>
concept Derived = std::is_base_of_v<T1, T2>;

template <typename T>
inline std::ostream& operator<<(std::ostream& out, const Vec3<T>& v) {
  return out << v.x() << " " << v.y() << " " << v.z();
}

template <typename T>
inline std::ostream& operator<<(std::ostream& out, const Vec2<T>& v) {
  return out << v.x() << " " << v.y();
}

template <typename T, size_t N, typename Fn>
inline Vec<T, N> compose(const Vec<T, N>& v1, const Vec<T, N>& v2,
                         Fn fn) requires Derived<std::binary_function<T, T, T>, Fn> {
  Vec<T, N> v;
  for (size_t i = 0; i < N; ++i) {
    v[i] = fn(v1[i], v2[i]);
  }
  return v;
}

template <typename T, size_t N>
inline Vec<T, N> operator-(const Vec<T, N>& v1, const Vec<T, N>& v2) {
  return compose(v1, v2, std::minus<T>{});
}

template <typename T, size_t N>
inline Vec<T, N> operator+(const Vec<T, N>& v1, const Vec<T, N>& v2) {
  return compose(v1, v2, std::plus<T>{});
}

template <typename T, size_t N>
inline Vec<T, N> operator*(const Vec<T, N>& v1, const T& c) {
  return compose(v1, Vec<T, N>(c), std::multiplies<T>{});
}

template <typename T, size_t N>
inline Vec<T, N> operator/(const Vec<T, N>& v1, const T& c) {
  return compose(v1, Vec<T, N>(c), std::divides<T>{});
}

template <typename T, size_t N>
inline T dot(const Vec<T, N>& v1, const Vec<T, N>& v2) {
  T sum = T(0);
  for (size_t i = 0; i < N; ++i) {
    sum += v1[i] * v2[i];
  }
  return sum;
}

template <typename T2>
inline Vec3<T2> cross(const Vec3<T2>& v1, const Vec3<T2>& v2) {
  // clang-format off
  Vec3<T2> v = {
       v1.y()*v2.z() - v1.z()*v2.y(),
       v1.z()*v2.x() - v1.x()*v2.z(),
       v1.x()*v2.y() - v1.y()*v2.x()
  };
  // clang-format on

  return v;
}

template <typename T, size_t N>
inline Vec<T, N> normalize(const Vec<T, N>& v) {
  return v / v.norm();
}

}  // namespace rtow