#pragma once

#include <vector>

#include "tile.hpp"

namespace core {
class Bag {
 public:
  Bag();

  void AddTile(const Tile& tile);

  void AddTiles(const std::vector<Tile>& tiles);

  Tile DrawTile();

  void Shuffle();

  void PrintBagInfo() const;

  int num_tiles_remanining() const {
    return static_cast<int>(tile_bag_.size());
  }

 private:
  static constexpr void addTileToBag(std::vector<Tile>& tile_bag, char letter,
                                     int points, int count);

  std::vector<Tile> tile_bag_;
};
}  // namespace core