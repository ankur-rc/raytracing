#pragma once
#include "rtow/hittable.hpp"
#include "rtow/ray.hpp"
#include "rtow/vec_utils.hpp"

namespace rtow {

template <typename T = float>
class Sphere : public Hittable<T> {
public:
  Sphere(const Vec3<T>& center, const T radius)
      : center_(center)
      , radius_(radius) {}

  bool hit(const Ray<T>& ray, const T t_min, const T t_max, HitRecord<T>& record) const override {
    const Vec3<T> oc = ray.origin() - center_;
    const auto a = dot(ray.direction(), ray.direction());
    const auto b = T(2) * dot(oc, ray.direction());
    const auto c = dot(oc, oc) - radius_ * radius_;
    const auto discriminant = b * b - 4 * a * c;
    if (discriminant < T(0)) return false;

    // find nearest (to ray) root within acceptable range
    const auto d = sqrt(discriminant);
    auto root = (-b - d) / (T(2) * a);
    if (root < t_min || root > t_max) {
      root = (-b + d) / (T(2) * a);
      if (root < t_min || root > t_max) {
        return false;
      }
    }

    // update hit record
    const Vec3<T> p = ray.at(root);
    const Vec3<T> outside_normal = normalize(p - center_);
    record.Update(p, outside_normal, root, ray);

    return true;
  }

private:
  Vec3<T> center_ = {Vec3<T>::NaN};
  T radius_ = Vec3<T>::NaN;
};

}  // namespace rtow