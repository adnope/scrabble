#include "bag.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "tile.hpp"

namespace core {
Bag::Bag() {
  // The chosen letter distribution: <letter> <points> <quantity>
  std::string default_bag =
      "? 0 2\n"
      "a 1 9\n"
      "b 3 2\n"
      "c 3 2\n"
      "d 2 4\n"
      "e 1 12\n"
      "f 4 2\n"
      "g 2 3\n"
      "h 4 2\n"
      "i 1 9\n"
      "j 8 1\n"
      "k 5 1\n"
      "l 1 4\n"
      "m 3 2\n"
      "n 1 6\n"
      "o 1 8\n"
      "p 3 2\n"
      "q 10 1\n"
      "r 1 6\n"
      "s 1 4\n"
      "t 1 6\n"
      "u 1 4\n"
      "v 4 2\n"
      "w 4 2\n"
      "x 8 1\n"
      "y 4 2\n"
      "z 10 1";

  // Populate the tile_bag (shuffle for randomization)
  std::istringstream bag_stream(default_bag);
  std::string line;
  while (std::getline(bag_stream, line)) {
    std::istringstream line_stream(line);
    char letter{};
    int points{};
    int count{};
    line_stream >> letter >> points >> count;

    addTileToBag(tile_bag_, letter, points, count);
  }

  Shuffle();
}

constexpr void Bag::addTileToBag(std::vector<Tile>& tile_bag, const char letter,
                                 const int points, const int count) {
  for (int i = 0; i < count; ++i) {
    tile_bag.emplace_back(static_cast<char>(toupper(letter)), points);
  }
}

void Bag::AddTile(const Tile& tile) {
  tile_bag_.emplace_back(tile);
  Shuffle();
}

void Bag::AddTiles(const std::vector<Tile>& tiles) {
  tile_bag_.insert(tile_bag_.end(), tiles.begin(), tiles.end());
}

Tile Bag::DrawTile() {
  Tile drawn_tile = tile_bag_.back();
  tile_bag_.pop_back();

  Shuffle();

  return drawn_tile;
}

void Bag::Shuffle() {
  std::random_device rd;
  std::mt19937 rng(rd());
  std::shuffle(tile_bag_.begin(), tile_bag_.end(), rng);
}

void Bag::PrintBagInfo() const {
  std::cout << "[Bag]\n";
  std::cout << "Bag content: \n";
  for (const auto& tile : tile_bag_) {
    tile.PrintInfo();
  }
  std::cout << '\n';
  std::cout << "Tiles remaining: " << num_tiles_remanining() << '\n';
}
}  // namespace core
