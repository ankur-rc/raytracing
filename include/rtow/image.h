#pragma once
#include <iostream>
#include <vector>

#include "rtow/color.h"

namespace rtow {

enum class PIXEL_FORMAT { UNKNOWN = -1, RGB = 0 };

struct Image {
  Image(const size_t width, const size_t height, const PIXEL_FORMAT pf)
      : width_(width)
      , height_(height)
      , pf_(pf) {}

  bool alloc() {
    data_.resize(width_ * height_, color::constant(0.));
    return true;
  }

  color* data() { return data_.data(); }

  const color* data() const { return data_.data(); }

  size_t width() const { return width_; }
  size_t height() const { return height_; }
  PIXEL_FORMAT format() const { return pf_; }

  color& at(const size_t u, const size_t v) { return data_.at(v * width_ + u); }
  const color& at(const size_t u, const size_t v) const { return data_.at(v * width_ + u); }

private:
  size_t width_ = 0;
  size_t height_ = 0;
  std::vector<color> data_;
  PIXEL_FORMAT pf_ = PIXEL_FORMAT::UNKNOWN;
};

void to_ppm(const Image& image, std::ostream& out, std::ostream& log = std::cout, bool write_header = true);
}  // namespace rtow