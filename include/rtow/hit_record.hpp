#pragma once

namespace rtow {

template <typename T>
class Material;

template <typename T>
struct HitRecord {
  Vec3<T> p;
  Vec3<T> n;  // always points against the direction of the ray
  T t = std::numeric_limits<T>::quiet_NaN();
  bool front_face = true;
  std::shared_ptr<Material<T>> material_ptr = nullptr;

  void Update(const Vec3<T>& p, const Vec3<T>& outward_normal, const T t, const Ray<T>& ray,
              const std::shared_ptr<Material<T>>& material_ptr) {
    this->p = p;
    this->t = t;
    this->front_face =
        dot(ray.direction(), outward_normal) < T(0);  // ray intersects from outside; i.e on 'front-face'
    this->n = front_face ? outward_normal : -outward_normal;
    this->material_ptr = material_ptr;
  }
};

}  // namespace rtow