#pragma once

#include <cstdint>

#include "tile.hpp"

namespace core {
class Square {
 public:
  enum class Symbol : uint8_t {
    kStart,
    kNormal,
    kDoubleLetter,
    kDoubleWord,
    kTripleLetter,
    kTripleWord
  };

  enum class Multiplier : uint8_t {
    kNormal,
    kDoubleLetter,
    kDoubleWord,
    kTripleLetter,
    kTripleWord
  };

  Square() : row_{}, col_{}, has_tile_{}, symbol_{1}, multiplier_{} {}

  Square(int row, int col, Symbol symbol, Multiplier multiplier);

  void PlaceTile(Tile t);

  bool IsOccupied() const { return has_tile_; }

  char tile_letter() const;

  int tile_points() const { return tile_.points(); }

  Symbol symbol() const { return symbol_; }

  Multiplier multiplier() const { return multiplier_; }

 private:
  int row_, col_;

  Tile tile_;
  bool has_tile_;

  Symbol symbol_;
  Multiplier multiplier_;
};
}  // namespace core
