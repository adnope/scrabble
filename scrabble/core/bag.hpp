#pragma once

#include <vector>

#include "tile.hpp"

namespace core {
class Bag {
 public:
  Bag();
  
  void AddTiles(const Tile& tile);

  void AddTiles(const std::vector<Tile>& tiles);

  std::vector<Tile> DrawTiles(int number_of_tiles);

  int GetNumberOfTilesRemaining() const;

  void Shuffle();

  void PrintBagInfo() const;

 private:
  std::vector<Tile> tile_bag_;
};
}  // namespace core