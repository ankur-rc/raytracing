#include <chrono>
#include <fstream>

#include "rtow/color.h"
#include "rtow/image.h"

int main(int argc, char** argv) {
  size_t width = 480;
  size_t height = 360;
  rtow::Image img(width, height, rtow::PIXEL_FORMAT::RGB);
  std::string file_path = "main.ppm";
  std::ofstream out(file_path, std::ios_base::out);
  std::ostream& logging = std::cout;

  if (!img.alloc()) {
    return -1;
  }

  const int64_t time_start = std::chrono::system_clock::now().time_since_epoch().count();
  for (size_t i = 0; i < img.height(); ++i) {
    for (size_t j = 0; j < img.width(); ++j) {
      {
        rtow::color& col = img.at(j, i);
        col.x() = (i + 1) / static_cast<float>(img.height());
        col.y() = (j + 1) / static_cast<float>(img.width());
        col.z() = 0.25;
      }
    }
  }
  const int64_t time_end = std::chrono::system_clock::now().time_since_epoch().count();

  logging << "Took " << (time_end - time_start) * 1e-6 << "ms to complete rendering\n";

  logging << "Writing image ...\n";
  rtow::to_ppm(img, out, logging);
  logging << "completed\n";

  return 0;
}