#include <iostream>
#include <vector>

#include "rtow/matrix_utils.hpp"
#include "rtow/vec_utils.hpp"

int main(int argc, char** argv) {
  std::vector<rtow::Mat3d> mat3 = {
      rtow::Mat3d::constant(6.), rtow::Mat3d::random(-3., 3.), {{1., 0., 0., 0., 1., 0., 0., 0., 1.}}};
  std::vector<rtow::Matrix<double, 3, 4>> mat3x4 = {{{1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12.}},
                                                    rtow::Matrix<double, 3, 4>::constant(6.),
                                                    rtow::Matrix<double, 3, 4>::random(-3., 3.)};
  std::vector<rtow::Vec3d> vec = {{1.}, {1., -2., 3.}, {1, 0, 0}};

  for (size_t i = 0; i < 3; ++i) {
    auto& A = mat3[i];
    auto& B = mat3x4[i];
    auto& v = vec[i];
    std::cout << "A = mat3[" << i << "] = \n" << A.Print() << "\n";
    std::cout << "B = mat3x4[" << i << "] = \n" << B.Print() << "\n";
    std::cout << "v = vec[" << i << "] = \n" << v.Print() << "\n\n";
    std::cout << "-A      =\n"
              << (-A).Print() << "\n"
              << "-B      =\n"
              << (-B).Print() << "\n"
              << "A - A   =\n"
              << (A - A).Print() << "\n"
              << "B - B   =\n"
              << (B - B).Print() << "\n"
              << "A + A   =\n"
              << (A + A).Print() << "\n"
              << "B + B   =\n"
              << (B + B).Print() << "\n"
              << "A*3     =\n"
              << (A * 3.0).Print() << "\n"
              << "B*3     =\n"
              << (B * 3.0).Print() << "\n"
              << "A / 3   =\n"
              << (A / 3.0).Print() << "\n"
              << "B / 3   =\n"
              << (B / 3.0).Print() << "\n"
              << "A x B   =\n"
              << (A * B).Print() << "\n"
              << "A x v   =\n"
              << (A * v).Print() << "\n\n"
              << "B.t     =\n"
              << rtow::transpose(B).Print() << "\n"
              << "B.t x v =\n"
              << (rtow::transpose(B) * v).Print() << "\n"

              << "\n";
  }

  return 0;
}