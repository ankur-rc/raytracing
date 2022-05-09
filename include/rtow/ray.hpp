#pragma once

#include "rtow/vec_utils.hpp"

namespace rtow {
template <typename T>
class Ray {
public:
  Ray() = default;
  Ray(const Vec3<T>& origin, const Vec3<T>& direction)
      : origin_(origin)
      , direction_(direction) {}

  const Vec3<T>& origin() const { return origin_; }
  const Vec3<T>& direction() const { return direction_; }

  Vec3<T> at(const T t) const { return origin_ + direction_ * t; }

private:
  Vec3<T> origin_ = {Vec3<T>::NaN};
  Vec3<T> direction_ = {Vec3<T>::NaN};
};

using Rayf = Ray<float>;
using Rayd = Ray<double>;
}  // namespace rtow