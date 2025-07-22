#include <iostream>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <fstream>
#include <string>

#include "core/bag.hpp"
#include "core/dictionary.hpp"
#include "core/player.hpp"
#include "core/tile.hpp"
#include "doctest/doctest.h"
#include "spdlog/spdlog.h"

TEST_CASE("Dictionary test") {
  SUBCASE("CSW") {
    core::Dictionary dictionary(core::Dictionary::CSW);
    std::ifstream csw6_stream("assets/dictionaries/csw6.dict");

    if (!csw6_stream.is_open()) {
      spdlog::error("Failed loading dictionary: assets/dictionaries/csw6.dict");
      return;
    }

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
  SUBCASE("TWL") {
    core::Dictionary dictionary(core::Dictionary::TWL);
    std::ifstream twl6_stream("assets/dictionaries/twl6.dict");

    if (!twl6_stream.is_open()) {
      spdlog::error("Failed loading dictionary: assets/dictionaries/twl6.dict");
      return;
    }

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

TEST_CASE("Player test") {
  using core::Bag;
  using core::Tile;

  Bag bag;
  core::Player player("test_player_1", 0);
  // std::vector<Tile> initial_tiles = {{'A', 1}, {'B', 3}, {'C', 3}, {'D', 2},
  //                                    {'E', 1}, {'F', 4}, {'G', 2}};
  player.DrawNewTiles(bag);
  // player.AddTiles(initial_tiles);
  spdlog::info("New player created");
  player.PrintDeck();
  std::cout << "Subcase: ";

  SUBCASE("Constructor") {
    std::cout << "[Player constructor]\n";

    CHECK(player.name() == "test_player_1");
    CHECK(player.score() == 0);
    CHECK(player.current_deck_size() == 7);
  }

  SUBCASE("Use tiles and draw tiles") {
    std::cout << "[Player use tiles and draw tiles]\n";

    player.UseTile(1);
    player.UseTile(4);
    player.UseTile(5);
    std::cout << "Deck before: \n";
    player.PrintDeck();
    CHECK(player.current_deck_size() == 4);

    player.DrawNewTiles(bag);
    std::cout << "Deck before: \n";
    player.PrintDeck();
    CHECK(player.current_deck_size() == 7);
  }

  SUBCASE("Swapping") {
    std::cout << "[Player swap]\n";
    std::cout << "Deck before: \n";
    player.PrintDeck();

    player.PerformSwap(bag, {1, 2, 3});

    std::cout << "Deck after: \n";
    player.PrintDeck();

    CHECK(player.current_deck_size() == 7);
  }
}