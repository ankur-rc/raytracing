#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>

#include "rtow/camera.hpp"
#include "rtow/color.h"
#include "rtow/hittable.hpp"
#include "rtow/image.h"
#include "rtow/material.hpp"
#include "rtow/sphere.hpp"
#include "rtow/utils.hpp"
#include "rtow/vec_utils.hpp"

using namespace rtow;

color ray_color(const Rayf& r, const HittableList<float>& world, const size_t max_bounces) {
  HitRecord<float> record;
  Rayf ray_out, ray_in = r;

  size_t depth = max_bounces;
  color attenuation, col = {1.F};
  bool hit_once = false;

  while (depth > 0) {
    if (world.hit(ray_in, 0.001, 1000., record)) {
      hit_once = true;
      if (record.material_ptr->scatter(ray_in, record, attenuation, ray_out)) {
        col *= attenuation;
        ray_in = ray_out;
        depth = depth - 1;
      } else {
        // light ray got absorbed
        col = {0.F};
        break;
      }
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

int main(int argc, char** argv) {
  struct Profile {
    std::string name = "undefined";
    size_t width = 480;
    size_t height = 360;
    size_t samples_per_pixel = 64;
    size_t max_ray_bounces = 100;
    std::string string() {
      std::stringstream ss;
      ss << "Name: " << name << "\n"
         << "Width: " << width << "\n"
         << "Height: " << height << "\n"
         << "Samples per pixel: " << samples_per_pixel << "\n"
         << "Max bounces: " << max_ray_bounces << "\n";
      return ss.str();
    }
  };

  // clang-format off
  const Profile low    = {.name = "low",    .width = 160,  .height = 128,  .samples_per_pixel = 16,  .max_ray_bounces = 10 };
  const Profile medium = {.name = "medium", .width = 480,  .height = 360,  .samples_per_pixel = 64,  .max_ray_bounces = 100 };
  const Profile high   = {.name = "high",   .width = 1920, .height = 1080, .samples_per_pixel = 256, .max_ray_bounces = 1000 };
  // clang-format on

  Profile selected_profile = low;
  if (argc > 1) {
    std::string profile = std::string(argv[1]);
    if (profile == "--low" || profile == "-l") {
      selected_profile = low;
    } else if (profile == "--medium" || profile == "-m") {
      selected_profile = medium;
    } else if (profile == "--high" || profile == "-h") {
      selected_profile = high;
    }
  }

  std::string file_path = "part9.ppm";
  std::ostream& logging = std::cout;

  file_path = file_path.substr(0, file_path.find_first_of('.'))
                  .append("-")
                  .append(selected_profile.name)
                  .append(".ppm");
  std::ofstream out(file_path, std::ios_base::out);

  const size_t width = selected_profile.width;
  const size_t height = selected_profile.height;
  const size_t kSpp = selected_profile.samples_per_pixel;
  const size_t kRayBounces = selected_profile.max_ray_bounces;

  logging << "Saving to file: " << file_path << " using profile: \n" << selected_profile.string();

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
  const float s = 0.;

  Vec<float, 5> parameters(Vec<float, 5>::NaN);
  parameters[0] = fx;
  parameters[1] = fy;
  parameters[2] = cx;
  parameters[3] = cy;
  parameters[4] = s;

  std::shared_ptr<Camera<>> camera = std::make_shared<PinholeCamera<>>(
      static_cast<float>(width), static_cast<float>(height), parameters.data());

  // materials
  auto mat_ground = std::make_shared<rtow::Lambertian<float>>(rtow::color{0.5, 0.5, 0.0});
  auto mat_center = std::make_shared<rtow::Lambertian<float>>(rtow::color{0.7, 0.3, 0.3});
  // auto mat_left = std::make_shared<rtow::Metal<float>>(rtow::color{0.8, 0.8, 0.8}, 1.0);
  auto mat_left = std::make_shared<rtow::Dielectric<float>>(1.5F);
  auto mat_right = std::make_shared<rtow::Metal<float>>(rtow::color{0.8, 0.6, 0.2}, 0.0);

  // objects
  rtow::HittableList<float> world;
  world.add(std::make_shared<rtow::Sphere<float>>(Vec3f{0., 100.5, 1.}, 100., mat_ground));
  world.add(std::make_shared<rtow::Sphere<float>>(Vec3f{0., 0., 1.}, 0.5, mat_center));
  world.add(std::make_shared<rtow::Sphere<float>>(Vec3f{-1., 0., 1.}, 0.5, mat_left));
  world.add(std::make_shared<rtow::Sphere<float>>(Vec3f{-1., 0., 1.}, -0.4, mat_left));
  world.add(std::make_shared<rtow::Sphere<float>>(Vec3f{1., 0., 1.}, 0.5, mat_right));

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
      col.x() = std::pow(col.x(), 0.4);
      col.y() = std::pow(col.y(), 0.4);
      col.z() = std::pow(col.z(), 0.4);
    }
  }
  const int64_t time_end = std::chrono::system_clock::now().time_since_epoch().count();

  logging << "\nTook " << (time_end - time_start) * 1e-6 << "ms to complete rendering\n";

  logging << "Writing image ...";
  rtow::to_ppm(img, out, logging);
  logging << "completed\n";
}
