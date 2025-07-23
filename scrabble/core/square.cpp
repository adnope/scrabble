#include "square.hpp"

#include "tile.hpp"

namespace core {
Square::Square(const int row, const int col, Symbol symbol,
               Multiplier multiplier)
    : row_(row),
      col_(col),
      has_tile_(false),
      symbol_(symbol),
      multiplier_(multiplier) {}

void Square::PlaceTile(const Tile tile) {
  tile_ = tile;
  has_tile_ = true;
}

char Square::tile_letter() const {
  if (has_tile_) {
    if (tile_.IsBlankTile()) {
      return tile_.get_use();
    }
    return tile_.letter();
  }
  return 0;
}
}  // namespace core