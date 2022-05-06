#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>

#include "rtow/camera.hpp"
#include "rtow/color.h"
#include "rtow/hittable.hpp"
#include "rtow/image.h"
#include "rtow/sphere.hpp"
#include "rtow/utils.hpp"
#include "rtow/vec_utils.hpp"

using namespace rtow;

color ray_color(const Rayf& r, const HittableList<float>& world, const size_t recursion_depth) {
  HitRecord<float> record;
  Rayf outgoing_ray = r;
  size_t depth = recursion_depth;
  color col = {1.F};
  bool hit_once = false;

  while (depth > 0) {
    if (world.hit(outgoing_ray, 0.01, 1000., record)) {
      const Vec3f outgoing_direction = record.p + record.n + normalize(Vec3f::random(-1.F, 1.F));
      outgoing_ray = {record.p, outgoing_direction - record.p};
      col *= 0.5F;
      depth = depth - 1;
      hit_once = true;
    } else {
      // no-hit
      break;
    }
  }

  if (hit_once) {
    return col;
  }

  // background -- no-hit
  const Vec3f dir = normalize(r.direction());
  const float t = (dir.y() + .5f);
  return color(1.) * t + color(0.5f, 0.7f, 1.f) * (1.f - t);
}

int main() {
  std::string file_path = "part8.ppm";
  std::ofstream out(file_path, std::ios_base::out);
  std::ostream& logging = std::cout;

  const size_t width = 480;
  const size_t height = 360;
  constexpr size_t kSpp = 64;
  constexpr size_t kRayBounces = 16;

  // image
  Image img = {width, height, PIXEL_FORMAT::RGB};
  if (!img.alloc()) {
    std::cerr << "Failed to allocate image data\n";
    return -1;
  }

  // camera
  const float fx = width / 4.;
  const float fy = width / 4.;
  const float cx = width / 2.;
  const float cy = height / 2.;
  const float s = 0;

  Vec<float, 5> parameters(Vec<float, 5>::NaN);
  parameters[0] = fx;
  parameters[1] = fy;
  parameters[2] = cx;
  parameters[3] = cy;
  parameters[4] = s;

  std::shared_ptr<Camera<>> camera = std::make_shared<PinholeCamera<>>(
      static_cast<float>(width), static_cast<float>(height), parameters.data());

  // define scene
  rtow::HittableList<float> world;
  world.add(std::make_shared<rtow::Sphere<float>>(Vec3f{0., 0., 1.}, 0.5));
  world.add(std::make_shared<rtow::Sphere<float>>(Vec3f{0., 100.5, 1.}, 100.));

  // render
  const int64_t time_start = std::chrono::system_clock::now().time_since_epoch().count();
  for (size_t i = 0; i < img.height(); ++i) {
    for (size_t j = 0; j < img.width(); ++j) {
      logging << "\r"
              << "Progress: " << float(i * img.width() + j + 1) / float(img.width() * img.height()) * 100.F
              << "% (" << j << ", " << i << ")";
      rtow::color& col = img.at(j, i);
      col = color(0.F);
      for (size_t k = 0; k < kSpp; ++k) {
        const float ej = static_cast<float>(j) + rtow::random(0.F, 1.F);
        const float ei = static_cast<float>(i) + rtow::random(0.F, 1.F);
        const auto ray = camera->unproject({ej, ei});
        const color ray_col = ray_color(ray, world, kRayBounces);
        if (ray_col.has_NaN()) {
          logging << "For (" << ej << ", " << ei << ") and ray sample: " << k << " we got NAN\n";
        }
        col += ray_col;
      }
      col /= kSpp;
      // gamma correction
      col.x() = std::sqrt(col.x());
      col.y() = std::sqrt(col.y());
      col.z() = std::sqrt(col.z());
    }
  }
  const int64_t time_end = std::chrono::system_clock::now().time_since_epoch().count();

  logging << "\nTook " << (time_end - time_start) * 1e-6 << "ms to complete rendering\n";

  logging << "Writing image ...\n";
  rtow::to_ppm(img, out, logging);
  logging << "completed\n";
}
