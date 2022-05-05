#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>

#include "rtow/camera.hpp"
#include "rtow/color.h"
#include "rtow/image.h"
#include "rtow/vec_utils.hpp"

using namespace rtow;

color ray_color(const Rayf& r) {
  const Vec3f dir = normalize(r.direction());
  const float t = 0.5 * (dir.y() + 1.);
  return color(1.) * (1.f - t) + color(0.5f, 0.7f, 1.f) * t;
}

int main() {
  std::string file_path = "part4.ppm";
  std::ofstream out(file_path, std::ios_base::out);
  std::ostream& logging = std::cout;

  const size_t width = 1024;
  const size_t height = 768;

  // image
  Image img = {width, height, PIXEL_FORMAT::RGB};
  if (!img.alloc()) {
    std::cerr << "Failed to allocate image data\n";
    return -1;
  }

  // camera
  const float fx = width;
  const float fy = width;
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

  // render
  const int64_t time_start = std::chrono::system_clock::now().time_since_epoch().count();
  for (size_t i = 0; i < img.height(); ++i) {
    for (size_t j = 0; j < img.width(); ++j) {
      {
        rtow::color& col = img.at(j, i);
        const auto ray = camera->unproject({static_cast<float>(j), static_cast<float>(i)});
        col = ray_color(ray);
      }
    }
  }
  const int64_t time_end = std::chrono::system_clock::now().time_since_epoch().count();

  logging << "Took " << (time_end - time_start) * 1e-6 << "ms to complete rendering\n";

  logging << "Writing image ...\n";
  rtow::to_ppm(img, out, logging);
  logging << "completed\n";
}
