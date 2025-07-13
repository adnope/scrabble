#pragma once

#include <string>

#include "tile.hpp"

namespace core {
class Square {
 public:
  Square() : row{}, col{}, hasTile{} {}

  Square(int row, int col, const std::string& symbol,
         const std::string& multiplier);

  std::string GetValue();

  std::string GetMultiplier();

  std::string ValueForBoard();

  void PlaceTile(Tile t);

  int GetTilePoints();

  bool IsOccupied() const;

 private:
  int row, col;

  Tile tile;
  bool hasTile;

  // Symbol includes: '***' (start square), '...' (normal square), '2L' (double
  // letter), '3L' (triple letter), '2W' (double word), '3W' (triple word)
  std::string symbol;

  // Multiplier includes: '1' (normal), '2' (double letter), '3' (triple
  // letter), '2N' (double word), '3N' (triple word)
  std::string multiplier;
};
}  // namespace core
