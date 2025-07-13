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
static constexpr void addTileToBag(std::vector<Tile>& tile_bag,
                                   const char letter, const int points,
                                   const int count) {
  for (int i = 0; i < count; ++i) {
    tile_bag.emplace_back(static_cast<char>(toupper(letter)), points);
  }
}

Bag::Bag() {
  // The chosen letter distribution: <letter> <points> <letter count>
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

    addTileToBag(tile_bag, letter, points, count);
  }

  Shuffle();
}

void Bag::AddTiles(const Tile& tile) { tile_bag.emplace_back(tile); }

void Bag::AddTiles(const std::vector<Tile>& tiles) {
  tile_bag.insert(tile_bag.end(), tiles.begin(), tiles.end());
}

/**
  Return a vector of tiles with letters in uppercase. This takes tiles from the
  end of tile_bag.
*/
std::vector<Tile> Bag::DrawTiles(const int number_of_tiles) {
  constexpr int MAX_TILES = 7;
  if (number_of_tiles > MAX_TILES || number_of_tiles < 0) {
    std::cerr << "Invalid tiles draw. You must only draw from 0 to 7 tiles.\n";
    return {};
  }
  std::vector<Tile> drawn_tiles;
  for (int i = 0; i < number_of_tiles && GetNumberOfTilesRemaining() > 0; ++i) {
    drawn_tiles.emplace_back(tile_bag.back());
    tile_bag.pop_back();
  }

  return drawn_tiles;
}

int Bag::GetNumberOfTilesRemaining() const {
  return static_cast<int>(tile_bag.size());
}

void Bag::Shuffle() {
  std::random_device rd;
  std::mt19937 rng(rd());
  std::shuffle(tile_bag.begin(), tile_bag.end(), rng);
}

void Bag::PrintBagInfo() const {
  std::cout << "Bag content: \n";
  for (const auto& tile : tile_bag) {
    tile.PrintTileInfo();
  }
  std::cout << "\nTiles remaining: " << GetNumberOfTilesRemaining() << '\n';
}
}  // namespace core