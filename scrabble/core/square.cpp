#include "square.hpp"

#include <iostream>
#include <string>

#include "tile.hpp"

namespace core {
Square::Square(int row, int col, const std::string& symbol,
               const std::string& multiplier)
    : row(row),
      col(col),
      hasTile(false),
      symbol(symbol),
      multiplier(multiplier) {}

std::string Square::GetValue() {
  if (hasTile) {
    if (tile.IsBlank()) {
      std::cout << tile.GetUse() << '\n';
      return std::string{tile.GetUse()};
    }
    return std::string{tile.GetLetter()};
  }
  return symbol;
}

std::string Square::ValueForBoard() {
  if (hasTile) {
    if (tile.IsBlank()) {
      return std::string{tile.GetUse()} + "0 ";
    }
    std::string temp =
        std::string{tile.GetLetter()} + std::to_string(tile.GetPoints());
    if (temp == "Z10") {
      return temp;
    }
    return temp + " ";
  }
  return symbol;
}

void Square::PlaceTile(Tile t) {
  tile = t;
  hasTile = true;
}

int Square::GetTilePoints() { return tile.GetPoints(); }

std::string Square::GetMultiplier() { return multiplier; }

bool Square::IsOccupied() const { return hasTile; }
}  // namespace core