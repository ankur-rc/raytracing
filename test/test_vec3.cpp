#include <iostream>
#include <vector>

#include "rtow/vec3.hpp"
#include "rtow/vec3_utils.hpp"

int main(int argc, char** argv) {
  const std::vector<rtow::Vec3d> v = {{1.}, {1., -2., 3.}};

  for (size_t i = 0; i < v.size(); ++i) {
    std::cout << "v[" << i + 1 << "]:"
              << "\n";
    std::cout << "v    =" << v[i].Print() << "\n"
              << "-v   =" << (-v[i]).Print() << "\n"
              << "v-v  =" << (v[i] - v[i]).Print() << "\n"
              << "v+v  =" << (v[i] + v[i]).Print() << "\n"
              << "v*3  =" << (v[i] * 3.0).Print() << "\n\n";
  }

  return 0;
}