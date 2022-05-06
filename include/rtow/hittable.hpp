#pragma once

#include <memory>
#include <vector>

#include "rtow/ray.hpp"

namespace rtow {
template <typename T>
struct HitRecord {
  Vec3<T> p;
  Vec3<T> n;  // always points aginst the direction of the ray
  T t = std::numeric_limits<T>::quiet_NaN();
  bool front_face = true;

  void Update(const Vec3<T>& p, const Vec3<T>& outward_normal, const T t, const Ray<T>& ray) {
    this->p = p;
    this->t = t;
    this->front_face =
        dot(ray.direction(), outward_normal) < T(0);  // ray intersects from outside; i.e on 'front-face'
    this->n = front_face ? outward_normal : -outward_normal;
  }
};

template <typename T>
class Hittable {
public:
  virtual bool hit(const Ray<T>& ray, const T t_min, const T t_max, HitRecord<T>& record) const = 0;
};

template <typename T = float>
class HittableList : public Hittable<T> {
public:
  HittableList() = default;

  void add(const std::shared_ptr<Hittable<T>>& object) { objects_.push_back(object); }
  void clear() { objects_.clear(); }
  bool hit(const Ray<T>& ray, const T t_min, const T t_max, HitRecord<T>& record) const override {
    HitRecord<T> temp_rec;
    bool hit_anything = false;
    T closest_so_far = t_max + T(0.1);

    for (const auto& object_ptr : objects_) {
      if (object_ptr->hit(ray, t_min, t_max, temp_rec)) {
        if (temp_rec.t < closest_so_far) {
          hit_anything = true;
          closest_so_far = temp_rec.t;
          record = temp_rec;
        }
      }
    }

    return hit_anything;
  }

private:
  std::vector<std::shared_ptr<Hittable<T>>> objects_;
};
}  // namespace rtow