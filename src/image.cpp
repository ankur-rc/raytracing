#include "rtow/image.h"

namespace rtow {
void to_ppm(const Image& image, std::ostream& out, std::ostream& log) {
  out << "P3\n" << image.width() << " " << image.height() << "\n255\n";

  const size_t width = image.width();
  const size_t height = image.height();

  for (size_t i = 0; i < height; ++i) {
    for (size_t j = 0; j < width; ++j) {
      //   log << "Processing: " << i * width + j << "/" << width * height << "("
      //       << (i * width + j) / double(width * height) * 100. << "%)\n";
      const color& col = image.data()[i * width + j];
      //   log << "color at " << i << ", " << j << ": " << col.Print() << "\n";
      // Divide the color by the number of samples and gamma-correct for gamma=2.0.
      
      write_color(out, col);
    }
  }
}
}  // namespace rtow