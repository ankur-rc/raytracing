#pragma once

#include "rtow/color.h"
#include "rtow/hit_record.hpp"
#include "rtow/ray.hpp"
#include "rtow/vec_utils.hpp"

namespace rtow {

template <typename T>
class Material {
public:
  virtual bool scatter(const Ray<T>& ray_in, const HitRecord<T>& hit_record, color& attenuation,
                       Ray<T>& ray_out) const = 0;
};

template <typename T>
class Lambertian : public Material<T> {
public:
  Lambertian(const color& albedo)
      : albedo_(albedo) {}

  bool scatter(const Ray<T>& ray_in, const HitRecord<T>& hit_record, color& attenuation,
               Ray<T>& ray_out) const override {
    Vec3<T> scatter_direction = hit_record.n + normalize(Vec3<T>::random(T(-1), T(1)));
    // check for near zero
    scatter_direction = scatter_direction.norm() < T(1e-3) ? hit_record.n : scatter_direction;

    ray_out = {hit_record.p, scatter_direction};
    attenuation = albedo_;
    return true;
  }

private:
  color albedo_ = {color::NaN};
};

template <typename T>
class Metal : public Material<T> {
public:
  Metal(const color& albedo, const T fuzz_factor)
      : albedo_(albedo)
      , fuzz_factor_(fuzz_factor) {}

  bool scatter(const Ray<T>& ray_in, const HitRecord<T>& hit_record, color& attenuation,
               Ray<T>& ray_out) const override {
    const Vec3<T> reflected = reflect(ray_in.direction(), hit_record.n);
    ray_out = {hit_record.p, (reflected + fuzz_factor_ * random_in_unit_sphere<T>())};
    attenuation = albedo_;
    return (dot(ray_out.direction(), hit_record.n) > T(0.0001));
  }

private:
  color albedo_ = {color::NaN};
  T fuzz_factor_ = T(0);
};

}  // namespace rtow