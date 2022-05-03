#pragma once

#include "rtow/vec3.hpp"

namespace rtow {
using Vec3d = Vec3<double>;
using Vec3f = Vec3<float>;

template <typename T1, typename T2>
concept Derived = std::is_base_of_v<T1, T2>;

template <typename T>
inline std::ostream& operator<<(std::ostream& out, const Vec3<T>& v) {
  return out << v.x() << " " << v.y() << " " << v.z();
}

template <typename T, typename Fn>
inline Vec3<T> compose(const Vec3<T>& v1, const Vec3<T>& v2,
                       Fn fn) requires Derived<std::binary_function<T, T, T>, Fn> {
  Vec3<T> v = {fn(v1.x(), v2.x()), fn(v1.y(), v2.y()), fn(v1.z(), v2.z())};
  return v;
}

template <typename T2>
inline Vec3<T2> operator-(const Vec3<T2>& v1, const Vec3<T2>& v2) {
  return compose(v1, v2, std::minus<T2>{});
}

template <typename T2>
inline Vec3<T2> operator+(const Vec3<T2>& v1, const Vec3<T2>& v2) {
  return compose(v1, v2, std::plus<T2>{});
}

template <typename T2>
inline Vec3<T2> operator*(const Vec3<T2>& v1, const T2& c) {
  return compose(v1, Vec3<T2>(c), std::multiplies<T2>{});
}

template <typename T2>
inline Vec3<T2> operator/(const Vec3<T2>& v1, const T2& c) {
  return compose(v1, Vec3<T2>(c), std::divides<T2>{});
}

template <typename T2>
inline T2 dot(const Vec3<T2>& v1, const Vec3<T2>& v2) {
  return v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z();
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

template <typename T2>
inline Vec3<T2> normalize(const Vec3<T2>& v) {
  return v / v.norm();
}

}  // namespace rtow