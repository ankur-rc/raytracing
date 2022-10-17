#pragma once

#include "rtow/matrix.hpp"

namespace rtow {

template <typename T, size_t ROWS, size_t COLS>
Matrix<T, COLS, ROWS> transpose(const Matrix<T, ROWS, COLS>& A) {
  Matrix<T, COLS, ROWS> At;
  for (size_t j = 0; j < ROWS; ++j) {
    for (size_t i = 0; i < COLS; ++i) {
      At[At.linear_index(i, j)] = A[A.linear_index(j, i)];
    }
  }

  return At;
}

}  // namespace rtow