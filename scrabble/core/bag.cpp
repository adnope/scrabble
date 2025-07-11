#include "bag.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace core {
Bag::Bag() {
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

  // Read each line then populate bag_tiles with tiles
  std::istringstream bag_stream(default_bag);
  std::string line;
  while (std::getline(bag_stream, line)) {
    std::istringstream line_stream(line);
    char letter = 0;
    int points = 0;
    int count = 0;
    line_stream >> letter >> points >> count;

    for (int i = 0; i < count; ++i) {
      Tile tile(static_cast<char>(toupper(letter)), points);
      AddTiles(tile);
    }
  }

  Shuffle();
}

void Bag::AddTiles(const Tile& tile) {
  tiles_bag.push_back(tile);
  int j = static_cast<int>(rand() % tiles_bag.size());
  tiles_bag[tiles_bag.size() - 1] = tiles_bag[j];
  tiles_bag[j] = tile;
}

void Bag::AddTiles(const std::vector<Tile>& tiles) {
  for (const auto& tile : tiles) {
    AddTiles(tile);
  }
}

std::vector<Tile> Bag::DrawTiles(const int number_of_tiles) {
  constexpr int MAX_TILES = 7;
  if (number_of_tiles > MAX_TILES || number_of_tiles < 0) {
    std::cerr << "Invalid tiles draw\n";
    return {};
  }
  std::vector<Tile> drawn_tiles;
  for (int i = 0; i < number_of_tiles && GetNumberOfTilesRemaining() > 0; ++i) {
    drawn_tiles.emplace_back(tiles_bag.back());
    tiles_bag.pop_back();
  }

  return drawn_tiles;
}

int Bag::GetNumberOfTilesRemaining() const {
  return static_cast<int>(tiles_bag.size());
}

void Bag::Shuffle() {
  std::random_device rd;
  std::mt19937 rng(rd());
  std::shuffle(tiles_bag.begin(), tiles_bag.end(), rng);
}

void Bag::PrintBagInfo() const {
  std::cout << "Bag content: \n";
  for (const auto& tile : tiles_bag) {
    tile.PrintTileInfo();
  }
  std::cout << "\nTiles remaining: " << GetNumberOfTilesRemaining() << '\n';
}
}  // namespace core