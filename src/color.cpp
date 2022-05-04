#include "rtow/color.h"

namespace rtow {

void write_color(std::ostream& out, const color& col) {
  constexpr float SCALE = 255.9F;
  // clang-format off
  out << static_cast<int>(col.x() * SCALE) << ' ' 
      << static_cast<int>(col.y() * SCALE) << ' '
      << static_cast<int>(col.z() * SCALE) << '\n';
  // clang-format on
}

}  // namespace rtow