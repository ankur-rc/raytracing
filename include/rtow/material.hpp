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

template <typename T>
class Dielectric : public Material<T> {
public:
  Dielectric(const T refractive_index)
      : refractive_index_(refractive_index) {}

  bool scatter(const Ray<T>& ray_in, const HitRecord<T>& hit_record, color& attenuation,
               Ray<T>& ray_out) const override {
    attenuation = color(1.0);
    const T eta_in = hit_record.front_face ? T(1) : refractive_index_;
    const T eta_out = hit_record.front_face ? refractive_index_ : T(1);

    const Vec3<T> unit_dir = normalize(ray_in.direction());
    const T ct = std::min(dot(-unit_dir, hit_record.n), T(1));
    const T st = std::sqrt(T(1) - ct * ct);
    const bool do_reflect = ((eta_in / eta_out) * st) > T(1);

    if (do_reflect || reflectance(ct, refractive_index_) > random(T(0), T(1))) {
      const Vec3<T> reflected_dir = reflect(ray_in.direction(), hit_record.n);
      ray_out = {hit_record.p, reflected_dir};
    } else {
      const Vec3<T> refracted_dir = refract(normalize(ray_in.direction()), hit_record.n, eta_in, eta_out);
      ray_out = {hit_record.p, refracted_dir};
    }

    return true;
  }

private:
  T refractive_index_ = T(1);

  static T reflectance(const T ct, const T refractive_index) {
    // use Schlick's approximation for reflectance
    const T r0 = (T(1) - refractive_index) / (T(1) + refractive_index);
    const T r2 = r0 * r0;
    return r2 + (1 - r2) * std::pow((T(1) - ct), T(5));
  }
};

}  // namespace rtow