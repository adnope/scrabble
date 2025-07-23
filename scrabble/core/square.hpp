#pragma once

#include <cstdint>

#include "core/tile.hpp"
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

  Square() : is_occupied_{}, symbol_{Symbol::kNormal}, multiplier_{} {}
  
  Square(Symbol symbol, Multiplier multiplier);

  void PlaceTile(Tile t);

  bool IsOccupied() const { return is_occupied_; }

  Tile tile() const { return tile_; }

  char tile_letter() const;

  int tile_points() const { return tile_.points(); }

  Symbol symbol() const { return symbol_; }

  Multiplier multiplier() const { return multiplier_; }

 private:
  Tile tile_;
  bool is_occupied_;

  Symbol symbol_;
  Multiplier multiplier_;
};
}  // namespace core
