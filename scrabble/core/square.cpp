#include "square.hpp"

#include <string>

#include "tile.hpp"

namespace core {
Square::Square(const int row, const int col, const std::string& symbol,
               const std::string& multiplier)
    : row_(row),
      col_(col),
      has_tile_(false),
      symbol_(symbol),
      multiplier_(multiplier) {}

void Square::PlaceTile(const Tile t) {
  tile_ = t;
  has_tile_ = true;
}

std::string Square::value() {
  if (has_tile_) {
    if (tile_.IsBlank()) {
      return std::string{tile_.get_use()};
    }
    return std::string{tile_.letter()};
  }
  return symbol_;
}

std::string Square::value_in_board() {
  if (has_tile_) {
    if (tile_.IsBlank()) {
      return std::string{tile_.get_use()} + "0 ";
    }
    std::string temp =
        std::string{tile_.letter()} + std::to_string(tile_.points());
    if (temp == "Z10") {
      return temp;
    }
    return temp + " ";
  }
  return symbol_;
}
}  // namespace core