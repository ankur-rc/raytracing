#pragma once

#include "rtow/ray.hpp"
#include "rtow/vec_utils.hpp"

namespace rtow {

enum class CameraType { UNKNOWN = -1, PINHOLE = 0 };

template <typename T = float>
class Camera {
public:
  Camera(const T width, const T height, const CameraType type, const T* const parameters)
      : width_(width)
      , height_(height)
      , type_(type) {
    switch (type) {
      case CameraType::PINHOLE:
        num_params_ = 5;
        break;
      case CameraType::UNKNOWN:
      default:
        num_params_ = 0;
    }

    for (size_t i = 0; i < num_params_; ++i) {
      parameters_[i] = parameters[i];
    }
  }

  const T& fu() const { return parameters_[0]; }
  const T& fv() const { return parameters_[1]; }
  const T& cu() const { return parameters_[2]; }
  const T& cv() const { return parameters_[3]; }
  const T& width() const { return width_; }
  const T& height() const { return height_; }
  const T& aspect() const { return width_ / height_; }
  const T* params() const { return parameters_.data(); }
  size_t model_size() const { return num_params_; }

  virtual Vec2<T> project(const Vec3<T>& pt3d) const = 0;
  virtual Ray<T> unproject(const Vec2<T>& uv) const = 0;

protected:
  static constexpr size_t kMaxParamSize = 10U;
  T width_ = T(0);
  T height_ = T(0);
  Vec<T, kMaxParamSize> parameters_ = {Vec<T, kMaxParamSize>::NaN};
  size_t num_params_ = 0;
  CameraType type_ = CameraType::UNKNOWN;
};

template <typename T = float>
class PinholeCamera : public Camera<T> {
public:
  PinholeCamera(const T width, const T height, const T* const parameters)
      : Camera<T>(width, height, CameraType::PINHOLE, parameters) {}

  Vec2<T> project(const Vec3<T>& pt3d) const override {
    Vec2<T> uv = {Vec2<T>::NaN};

    const T& fu = this->fu();
    const T& fv = this->fv();
    const T& cu = this->cu();
    const T& cv = this->cv();
    const T& s = this->parameters_[4];

    // K = [fu s cu; 0 fv cv; 0 0 1]
    // uv_homogeneous = K*pt3d
    uv[0] = (cu * pt3d[2] + fu * pt3d[0] + s * pt3d[1]) / pt3d[2];
    uv[1] = (cv * pt3d[2] + fv * pt3d[1]) / pt3d[2];

    return uv;
  }

  Ray<T> unproject(const Vec2<T>& uv) const override {
    Vec3<T> direction = {Vec3<T>::NaN};

    const T& fu = this->fu();
    const T& fv = this->fv();
    const T& cu = this->cu();
    const T& cv = this->cv();
    const T& s = this->parameters_[4];

    // K = [fu s cu; 0 fv cv; 0 0 1]
    // ray = (K_inverse*uv_homogenous)
    direction[0] = (uv[0] / fu) - ((cu * fv - cv * s) / (fu * fv)) - (s * uv[1]) / (fu * fv);
    direction[1] = (uv[1] / fv) - (cv / fv);
    direction[2] = static_cast<T>(1);

    // unit_ray = ray/ray.l2
    direction = normalize(direction);

    return Ray<T>{Vec3<T>{T(0.)}, direction};
  }
};

}  // namespace rtow