#include "square.hpp"

#include "tile.hpp"

namespace core {
Square::Square(Symbol symbol, Multiplier multiplier)
    : is_occupied_(false), symbol_(symbol), multiplier_(multiplier) {}

void Square::PlaceTile(const Tile tile) {
  tile_ = tile;
  is_occupied_ = true;
}

char Square::tile_letter() const {
  if (is_occupied_) {
    if (tile_.IsBlankTile()) {
      return tile_.get_use();
    }
    return tile_.letter();
  }
  return 0;
}
}  // namespace core
