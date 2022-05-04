#include <iostream>
#include <vector>

// #include "rtow/vec.hpp"
#include "rtow/vec_utils.hpp"

int main(int argc, char** argv) {
  const std::vector<rtow::Vec3d> v = {{1.}, {1., -2., 3.}, {1, 0, 0}};

  for (size_t i = 0; i < v.size(); ++i) {
    std::cout << "v[" << i + 1 << "]:"
              << "\n";
    std::cout << "v     =" << v[i].Print() << "\n"
              << "-v    =" << (-v[i]).Print() << "\n"
              << "v-v   =" << (v[i] - v[i]).Print() << "\n"
              << "v+v   =" << (v[i] + v[i]).Print() << "\n"
              << "v*3   =" << (v[i] * 3.0).Print() << "\n"
              << "v.-v  =" << dot(v[i], -v[i]) << "\n"
              << "vxv   =" << cross(v[i], {0, 1, 0}).Print() << "\n"
              << "v/|v| =" << normalize(v[i]).Print() << "\n"
              << "\n";
  }

  return 0;
}