#pragma once
#include "rtow/ray.hpp"
#include "rtow/vec_utils.hpp"

namespace rtow {
template <typename T>
bool hit_sphere(const Vec3<T>& center, T radius, const Ray<T>& ray) {
  const Vec3<T> oc = ray.origin() - center;
  const auto a = dot(ray.direction(), ray.direction());
  const auto b = T(2) * dot(oc, ray.direction());
  const auto c = dot(oc, oc) - radius * radius;
  const auto discriminant = b * b - 4 * a * c;
  return (discriminant > 0);
}
}  // namespace rtow