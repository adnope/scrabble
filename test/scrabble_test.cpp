#include <iostream>

#include "core/board.hpp"
#include "core/square.hpp"

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

void PrintBoardResponse(const core::Board::MoveValidationResponse& response) {
  std::cout << "[Move response]: \nWords: \n";
  for (const auto& word : response.words) {
    const auto content = word.content();
    for (const auto& [letter, mult] : content) {
      std::string multiplier;
      if (mult == core::Square::Multiplier::kDoubleLetter) {
        multiplier = "-DL";
      }
      if (mult == core::Square::Multiplier::kDoubleWord) {
        multiplier = "-DW";
      }
      if (mult == core::Square::Multiplier::kTripleLetter) {
        multiplier = "-TL";
      }
      if (mult == core::Square::Multiplier::kTripleWord) {
        multiplier = "-TW";
      }
      std::cout << letter << multiplier << '\n';
    }
  }
  std::cout << "Points: " << response.move_points << '\n';
  std::cout << "Status code: " << response.status_code << '\n';
}

TEST_CASE("Board test") {
  spdlog::info("Board test");
  core::Board board;
  core::Dictionary dictionary;

  std::vector<core::Board::Placement> initial_state_move = {
      {{'V', 4}, 5, 7},  {{'I', 1}, 5, 8}, {{'S', 1}, 5, 9}, {{'I', 1}, 5, 10},
      {{'T', 1}, 5, 11}, {{'T', 1}, 7, 4}, {{'A', 1}, 7, 5}, {{'N', 1}, 7, 6},
      {{'N', 1}, 7, 7},  {{'E', 1}, 7, 8}, {{'D', 2}, 7, 9}, {{'P', 3}, 2, 8},
      {{'R', 1}, 3, 8},  {{'A', 1}, 4, 8}, {{'S', 1}, 6, 8}};
  for (const auto& placement : initial_state_move) {
    board.PlaceTile(placement.tile, placement.row, placement.col);
  }
  std::cout << board.GetDisplayFormat();

  std::vector<core::Board::Placement> move = {{{'A', 1}, 6, 9},
                                              {{'T', 1}, 6, 10}};
  const auto board_response = board.ValidateMove(move, dictionary);
  PrintBoardResponse(board_response);
  std::cout << board.GetDisplayFormat();

  std::vector<core::Board::Placement> move2 = {{{'P', 3}, 4, 10}};
  const auto board_response2 = board.ValidateMove(move2, dictionary);
  PrintBoardResponse(board_response2);
  std::cout << board.GetDisplayFormat();
}