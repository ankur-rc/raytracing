#include "rtow/vec_utils.hpp"

namespace rtow {
using color = Vec3f;

// note: 'col' values will be scaled by a factor of 255.9
void write_color(std::ostream& out, const color& col);

}  // namespace rtow
