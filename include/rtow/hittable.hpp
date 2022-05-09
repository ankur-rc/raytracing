#pragma once

#include <memory>
#include <vector>

#include "rtow/material.hpp"
#include "rtow/ray.hpp"
namespace rtow {
template <typename T>
class Hittable {
public:
  Hittable() = default;
  Hittable(const std::shared_ptr<Material<T>>& material_ptr)
      : material_ptr_(material_ptr) {}

  virtual bool hit(const Ray<T>& ray, const T t_min, const T t_max, HitRecord<T>& record) const = 0;

protected:
  std::shared_ptr<Material<T>> material_ptr_ = nullptr;
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