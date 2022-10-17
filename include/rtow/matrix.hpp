#pragma once
#include <array>
#include <cmath>
#include <concepts>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>

#include "rtow/utils.hpp"
#include "rtow/vec.hpp"

namespace rtow {

template <size_t N>
concept IsNonzero = (N > 0);

template <typename T, size_t ROWS, size_t COLS>
requires IsFloating<T> && IsNonzero<ROWS> && IsNonzero<COLS>
class Matrix {
public:
  static constexpr T NaN = std::numeric_limits<T>::quiet_NaN();

  Matrix() = default;

  static Matrix<T, ROWS, COLS> constant(const T constant = T(NaN)) {
    Matrix<T, ROWS, COLS> mat;
    for (size_t i = 0; i < ROWS * COLS; ++i) {
      mat[i] = constant;
    }

    return mat;
  }

  static Matrix<T, ROWS, COLS> random(const T min, const T max) {
    Matrix<T, ROWS, COLS> mat;
    for (size_t i = 0; i < ROWS * COLS; ++i) {
      mat[i] = rtow::random(min, max);
    }

    return mat;
  }

  Matrix(const std::array<T, ROWS * COLS>& l) { this->data_ = l; }

  const T* data() const { return data_.data(); }
  T* data() { return data_.data(); }

  const size_t linear_index(const size_t row, const size_t col) const { return row * COLS + col; }
  const T& operator()(const size_t row, const size_t col) const { return data_.at(linear_index(row, col)); }
  T& operator()(const size_t row, const size_t col) { return data_.at(linear_index(row, col)); }
  T& operator[](const size_t linear_index) { return data_.at(linear_index); }
  const T& operator[](const size_t linear_index) const { return data_.at(linear_index); }

  // A = A + B
  template <typename T2>
  Matrix<T, ROWS, COLS>& operator+=(const Matrix<T2, ROWS, COLS>& mat1) {
    for (size_t i = 0; i < n_; ++i) {
      this->data_[i] += mat1[i];
    }
    return *this;
  }

  // C = A + B
  template <typename T2>
  Matrix<T, ROWS, COLS> operator+(const Matrix<T2, ROWS, COLS>& mat1) const {
    Matrix<T, ROWS, COLS> mat2;
    for (size_t i = 0; i < n_; ++i) {
      mat2[i] = this->data_[i] + mat1[i];
    }
    return mat2;
  }

  // B = -A
  Matrix<T, ROWS, COLS> operator-() const {
    Matrix<T, ROWS, COLS> mat;
    for (size_t i = 0; i < n_; ++i) {
      mat[i] = -this->data_[i];
    }
    return mat;
  }

  // // A = -A
  // Matrix<T, ROWS, COLS>& operator-() {
  //   for (size_t i = 0; i < n_; ++i) {
  //     this->data_[i] = -this->data_[i];
  //   }
  //   return *this;
  // }

  // C = A - B
  template <typename T2>
  Matrix<T, ROWS, COLS> operator-(const Matrix<T2, ROWS, COLS>& mat1) const {
    return *this + (-mat1);
  }

  // A = c x A
  template <typename T2>
  Matrix<T, ROWS, COLS>& operator*=(const T2& c) {
    for (size_t i = 0; i < n_; ++i) {
      this->data_[i] *= c;
    }
    return *this;
  }

  // B = c x A
  template <typename T2>
  Matrix<T, ROWS, COLS> operator*(const T2& c) const {
    Matrix<T, ROWS, COLS> B;
    for (size_t i = 0; i < n_; ++i) {
      B[i] = this->data_[i] * c;
    }
    return B;
  }

  // A =  A / c
  template <typename T2>
  Matrix<T, ROWS, COLS>& operator/=(const T2& c) {
    for (size_t i = 0; i < n_; ++i) {
      this->data_[i] /= c;
    }
    return *this;
  }

  // B =  A / c
  template <typename T2>
  Matrix<T, ROWS, COLS> operator/(const T2& c) const {
    Matrix<T, ROWS, COLS> B;
    for (size_t i = 0; i < n_; ++i) {
      B[i] = this->data_[i] / c;
    }
    return B;
  }

  // v2 = A x v
  template <typename T2, size_t ROWS2>
  Vec<T, ROWS> operator*(const Vec<T2, ROWS2>& v) const {
    static_assert(ROWS2 == COLS);

    Vec<T, ROWS> v2(T(0));
    for (size_t j = 0; j < ROWS; ++j) {
      T sum = T(0);
      for (size_t i = 0; i < ROWS2; ++i) {
        sum += v[i] * this->operator()(j, i);
      }
      v2[j] = sum;
    }

    return v2;
  }

  // C = A x B
  template <typename T2, size_t ROWS2, size_t COLS2>
  Matrix<T, ROWS, COLS2> operator*(const Matrix<T2, ROWS2, COLS2>& B) const {
    static_assert(ROWS2 == COLS);

    Matrix<T, ROWS, COLS2> C;
    for (size_t j = 0; j < ROWS; ++j) {
      for (size_t k = 0; k < COLS2; ++k) {
        T sum = T(0);
        for (size_t i = 0; i < COLS; ++i) {
          sum += B(i, k) * this->operator()(j, i);
        }
        C(j, k) = sum;
      }
    }

    return C;
  }

  bool has_NaN() const {
    for (size_t i = 0; i < n_; ++i) {
      if (std::isnan(data_[i])) {
        return true;
      }
    }

    return false;
  }

  std::string Print(size_t precison = 4) const {
    std::stringstream ss;
    ss << std::setprecision(precison);

    for (size_t j = 0; j < ROWS; ++j) {
      ss << "[ ";
      for (size_t i = 0; i < COLS - 1; ++i) {
        ss << std::to_string(data_[linear_index(j, i)]) << ", ";
      }
      ss << std::to_string(data_[linear_index(j, COLS - 1)]) << " ]\n";
    }

    return ss.str();
  }

private:
  const size_t rows_ = ROWS;
  const size_t cols_ = COLS;
  const size_t n_ = ROWS * COLS;
  std::array<T, ROWS * COLS> data_;
};

template <typename T>
using Mat3 = Matrix<T, 3, 3>;

template <typename T>
using Mat4 = Matrix<T, 4, 4>;

using Mat3d = Mat3<double>;
using Mat3f = Mat3<float>;
using Mat4d = Mat4<double>;
using Mat4f = Mat4<float>;

}  // namespace rtow
