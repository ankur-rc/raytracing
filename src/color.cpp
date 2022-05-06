#include "rtow/color.h"

namespace rtow {

void write_color(std::ostream& out, const color& col) {
  constexpr float SCALE = 255.99F;
  // clang-format off
  out << static_cast<int>(std::clamp(col.x(), 0.F, .9999F) * SCALE) << ' ' 
      << static_cast<int>(std::clamp(col.y(), 0.F, .9999F) * SCALE) << ' '
      << static_cast<int>(std::clamp(col.z(), 0.F, .9999F) * SCALE) << '\n';
  // clang-format on
}

}  // namespace rtow