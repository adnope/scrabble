#include "tile.hpp"

#include "iostream"

namespace core {
void Tile::PrintInfo() const { std::cout << letter_ << points_ << ' '; }
}  // namespace core
