#include "square.hpp"

#include <string>

#include "tile.hpp"

namespace core {
Square::Square(int row, int col, const std::string& symbol,
               const std::string& multiplier)
    : row_(row),
      col_(col),
      has_tile_(false),
      symbol_(symbol),
      multiplier_(multiplier) {}

std::string Square::GetValue() {
  if (has_tile_) {
    if (tile_.IsBlank()) {
      // std::cout << tile.GetUse() << '\n';
      return std::string{tile_.GetUse()};
    }
    return std::string{tile_.GetLetter()};
  }
  return symbol_;
}

std::string Square::ValueForBoard() {
  if (has_tile_) {
    if (tile_.IsBlank()) {
      return std::string{tile_.GetUse()} + "0 ";
    }
    std::string temp =
        std::string{tile_.GetLetter()} + std::to_string(tile_.GetPoints());
    if (temp == "Z10") {
      return temp;
    }
    return temp + " ";
  }
  return symbol_;
}

void Square::PlaceTile(Tile t) {
  tile_ = t;
  has_tile_ = true;
}

int Square::GetTilePoints() { return tile_.GetPoints(); }

std::string Square::GetMultiplier() { return multiplier_; }

bool Square::IsOccupied() const { return has_tile_; }
}  // namespace core