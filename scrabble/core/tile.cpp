#include "tile.hpp"

#include <iostream>

namespace core {
void Tile::PrintTileInfo() const { std::cout << letter << points << " "; }
}  // namespace core