#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>

#include "rtow/camera.hpp"
#include "rtow/color.h"
#include "rtow/hittable.hpp"
#include "rtow/image.h"
#include "rtow/sphere.hpp"
#include "rtow/vec_utils.hpp"

using namespace rtow;

color ray_color(const Rayf& r, const HittableList<float>& world) {
  HitRecord<float> record;
  if (world.hit(r, 0., 100., record)) {
    return (color{1., 1., 1.} + record.n) * 0.5f;
  }

  const Vec3f dir = normalize(r.direction());
  const float t = (dir.y() + .5f);
  return color(1.) * t + color(0.5f, 0.7f, 1.f) * (1.f - t);
}

int main() {
  std::string file_path = "part6.ppm";
  std::ofstream out(file_path, std::ios_base::out);
  std::ostream& logging = std::cout;

  const size_t width = 480;
  const size_t height = 360;

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
  // std::shared_ptr<rtow::Material<float>> mat_ground = std::make_shared

  rtow::HittableList<float> world;
  world.add(std::make_shared<rtow::Sphere<float>>(Vec3f{0., 0., 1.}, 0.5));
  world.add(std::make_shared<rtow::Sphere<float>>(Vec3f{0., 100.25, 1.}, 100.));

  // render
  const int64_t time_start = std::chrono::system_clock::now().time_since_epoch().count();
  for (size_t i = 0; i < img.height(); ++i) {
    for (size_t j = 0; j < img.width(); ++j) {
      {
        rtow::color& col = img.at(j, i);
        const auto ray = camera->unproject({static_cast<float>(j), static_cast<float>(i)});
        col = ray_color(ray, world);
      }
    }
  }
  const int64_t time_end = std::chrono::system_clock::now().time_since_epoch().count();

  logging << "Took " << (time_end - time_start) * 1e-6 << "ms to complete rendering\n";

  logging << "Writing image ...\n";
  rtow::to_ppm(img, out, logging);
  logging << "completed\n";
}
