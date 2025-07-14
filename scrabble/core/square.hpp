#pragma once

#include <string>

#include "tile.hpp"

namespace core {
class Square {
 public:
  Square() : row_{}, col_{}, has_tile_{} {}

  Square(int row, int col, const std::string& symbol,
         const std::string& multiplier);

  std::string GetValue();

  std::string GetMultiplier();

  std::string ValueForBoard();

  void PlaceTile(Tile t);

  int GetTilePoints();

  bool IsOccupied() const;

 private:
  int row_, col_;

  Tile tile_;
  bool has_tile_;

  // Symbol includes: '***' (start square), '...' (normal square), '2L' (double
  // letter), '3L' (triple letter), '2W' (double word), '3W' (triple word)
  std::string symbol_;

  // Multiplier includes: '1' (normal), '2' (double letter), '3' (triple
  // letter), '2N' (double word), '3N' (triple word)
  std::string multiplier_;
};
}  // namespace core
