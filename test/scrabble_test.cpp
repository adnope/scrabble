#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <fstream>
#include <string>
#include "core/bag.hpp"
#include "core/board.hpp"
#include "core/player.hpp"
#include "core/tile.hpp"
#include "core/dictionary.hpp"
#include "doctest/doctest.h"

TEST_CASE("dictionary test") {
  SUBCASE("csw") {
    core::Dictionary dictionary(core::Dictionary::CSW);
    std::ifstream csw6_stream("assets/dictionaries/csw6.dict");
    std::string line;
    int word_count = 0;
    int word_contained_count = 0;
    while (std::getline(csw6_stream, line)) {
      ++word_count;
      if (dictionary.Contains(line)) {
        ++word_contained_count;
      }
    }
    CHECK(word_count == word_contained_count);
  }
  SUBCASE("twl") {
    core::Dictionary dictionary(core::Dictionary::TWL);
    std::ifstream twl6_stream("assets/dictionaries/twl6.dict");
    std::string line;
    int word_count = 0;
    int word_contained_count = 0;
    while (std::getline(twl6_stream, line)) {
      ++word_count;
      if (dictionary.Contains(line)) {
        ++word_contained_count;
      }
    }
    CHECK(word_count == word_contained_count);
  }
}

TEST_CASE("Player functions test") {
  core::Bag bag;
  core::Board board;
  core::Dictionary dictionary(core::Dictionary::CSW);
  core::Player player("Test Player", 0, 7);
  std::vector<core::Tile> initial_tiles = {
      core::Tile('A', 1), core::Tile('B', 3), core::Tile('C', 3),
      core::Tile('D', 2), core::Tile('E', 1), core::Tile('F', 4),
      core::Tile('G', 2)};

<<<<<<< HEAD
  core::Bag bag1; 

  player.GetTiles(initial_tiles);
=======
  player.PutTilesInHand(initial_tiles);
>>>>>>> aaf321228cf4007f9fd4d53a9997dbc8ab8efa2b

  SUBCASE("Player constructor") {
    CHECK(player.name() == "Test Player");
    CHECK(player.score() == 0);
    CHECK(player.num_tiles_in_hand() == 7);
  }

  SUBCASE("UseTiles") {
    // Check tile on hand
    CHECK(player.num_tiles_in_hand() == 7);
    std::string init_tile;
    for (const auto& tile : initial_tiles) {
      init_tile += tile.get_use();
    }
    CHECK(init_tile == "ABCDEFG");
    // Use tiles
    player.UseTile('A');
    CHECK(player.num_tiles_in_hand() == 6);
  }

  SUBCASE("Execute Exchange Move") {
    // Get tiles from bag
    std::string word = "ABC";
    player.ExecuteExchangeMove(bag, word);
    CHECK(player.num_tiles_in_hand() == 7);

    //
  }
}
