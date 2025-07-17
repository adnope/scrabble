#pragma once

#include <vector>

#include "tile.hpp"

namespace core {
class Bag {
 public:
  Bag();
  
  void AddTiles(const Tile& tile);

  void AddTiles(const std::vector<Tile>& tiles);

  std::vector<Tile> DrawTiles(int num_tiles);

  void Shuffle();

  void PrintBagInfo() const;

  int num_tiles_remanining() const;

 private:
  std::vector<Tile> tile_bag_;
};
}  // namespace core