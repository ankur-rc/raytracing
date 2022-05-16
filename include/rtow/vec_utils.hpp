#pragma once

#include "rtow/utils.hpp"
#include "rtow/vec.hpp"
namespace rtow {

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
inline Vec<T, N> operator*(const Vec<T, N>& v1, const Vec<T, N>& v2) {
  return compose(v1, v2, std::multiplies<T>{});
}

template <typename T, size_t N>
inline Vec<T, N> operator*(const Vec<T, N>& v1, const T& c) {
  return compose(v1, Vec<T, N>(c), std::multiplies<T>{});
}

template <typename T, size_t N>
inline Vec<T, N> operator*(const T& c, const Vec<T, N>& v1) {
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

template <typename T>
inline Vec<T, 3> random_in_unit_sphere() {
  while (true) {
    auto p = Vec<T, 3>::random(T(-1), T(1));  // https://mathworld.wolfram.com/SpherePointPicking.html
    auto norm2 = p.norm_squared();
    if (norm2 > T(1) || norm2 < T(1e-6)) continue;
    return normalize(p);
  }
}
template <typename T>
inline Vec<T, 3> random_in_hemisphere(const Vec<T, 3>& normal) {
  Vec<T, 3> in_unit_sphere = random_in_unit_sphere<T>();
  if (dot(in_unit_sphere, normal) > 0.0)  // In the same hemisphere as the normal
    return in_unit_sphere;
  else
    return -in_unit_sphere;
}

template <typename T>
Vec3<T> reflect(const Vec3<T>& v, const Vec3<T>& n) {
  return v.norm() * (normalize(v) + T(2) * normalize(n));
}

template <typename T>
Vec3<T> refract(const Vec3<T>& ray_in, const Vec3<T>& normal, const T eta_in, const T eta_out) {
  const T ct = std::min(dot(-ray_in, normal), T(1));
  const Vec3<T> ray_out_perpendicular = (eta_in / eta_out) * (ray_in + ct * normal);
  const Vec3<T> ray_out_parallel = -std::sqrt(std::abs(T(1) - ray_out_perpendicular.norm_squared())) * normal;
  return ray_out_perpendicular + ray_out_parallel;
}

}  // namespace rtow