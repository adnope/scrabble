
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <iostream>
#include <string>

#include "cli/game_cli.hpp"
#include "core/bag.hpp"
#include "core/board.hpp"
#include "core/dictionary.hpp"
#include "core/lexicon.hpp"
#include "core/player.hpp"
#include "core/square.hpp"
#include "core/tile.hpp"
#include "doctest/doctest.h"
#include "spdlog/spdlog.h"

TEST_CASE("Lexicon contains test") {
  core::Lexicon lexicon;
  lexicon.AddWord("cat");
  lexicon.AddWord("car");
  lexicon.AddWord("dog");

  SUBCASE("Lexicon size") { CHECK(lexicon.size() == 8); }

  SUBCASE("Lexicon contains words") {
    CHECK(lexicon.Contains("cat") == true);
    CHECK(lexicon.Contains("car") == true);
    CHECK(lexicon.Contains("dog") == true);
  }

  SUBCASE("Lexicon contains prefixes") {
    CHECK(lexicon.ContainsPrefix("ca") == true);
    CHECK(lexicon.ContainsPrefix("do") == true);
    CHECK(lexicon.ContainsPrefix("fi") == false);
    CHECK(lexicon.ContainsPrefix("bi") == false);
  }
}

TEST_CASE("Lexicon preload test") {
  core::Lexicon lexicon;

  SUBCASE("Random word check with CSW dictionary") {
    lexicon.PreLoadDictionary(core::Dictionary::DictionaryType::CSW);
    CHECK(lexicon.Contains("aalii") == true);
    CHECK(lexicon.Contains("abactinally") == true);
    CHECK(lexicon.Contains("avitaminosis") == true);

    CHECK(lexicon.Contains("avifaunaszz") == false);
    CHECK(lexicon.Contains("axiomatizationsp") == false);
    CHECK(lexicon.Contains("penicillationsss") == false);
  }

  SUBCASE("Random word check with TWL dictionary") {
    lexicon.PreLoadDictionary(core::Dictionary::DictionaryType::TWL);
    CHECK(lexicon.Contains("aasvogel") == true);
    CHECK(lexicon.Contains("aasvogels") == true);
    CHECK(lexicon.Contains("trailheads") == true);
    CHECK(lexicon.Contains("traditionalists") == true);

    CHECK(lexicon.Contains("avifaunaszz") == false);
    CHECK(lexicon.Contains("axiomatizationsp") == false);
    CHECK(lexicon.Contains("penicillationsss") == false);
  }
}

TEST_CASE("Player test") {
  using core::Bag;
  using core::Tile;

  Bag bag;
  core::Player player("test_player_1", 0);
  std::vector<Tile> initial_tiles = {{'A', 1}, {'B', 3}, {'C', 3}, {'D', 2},
                                     {'E', 1}, {'F', 4}, {'G', 2}};
  player.AddTiles(initial_tiles);
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
  switch (response.status) {
    case core::Board::ResponseStatus::kSuccess: {
      std::cout << "Status: SUCCESS\n";
      break;
    }
    case core::Board::ResponseStatus::kOccupied: {
      std::cout << "Status: MOVE OCCUPIED\n";
      break;
    }
    case core::Board::ResponseStatus::kNotAligned: {
      std::cout << "Status: MOVE NOT ALIGNED\n";
      break;
    }
    case core::Board::ResponseStatus::kWordsInvalid: {
      std::cout << "Status: INVALID WORDS\n";
      break;
    }
  }
}

TEST_CASE("Board test") {
  spdlog::info("Board test");
  core::Board board;
  core::Lexicon lexicon;
  lexicon.PreLoadDictionary(core::Dictionary::CSW);

  std::vector<core::Board::Placement> initial_state_move = {
      {{'V', 4}, 5, 7},  {{'I', 1}, 5, 8}, {{'S', 1}, 5, 9}, {{'I', 1}, 5,
      10},
      {{'T', 1}, 5, 11}, {{'T', 1}, 7, 4}, {{'A', 1}, 7, 5}, {{'N', 1}, 7,
      6},
      {{'N', 1}, 7, 7},  {{'E', 1}, 7, 8}, {{'D', 2}, 7, 9}, {{'P', 3}, 2,
      8},
      {{'R', 1}, 3, 8},  {{'A', 1}, 4, 8}, {{'S', 1}, 6, 8}};
  for (const auto& placement : initial_state_move) {
    board.PlaceTile(placement.tile, placement.row, placement.col);
  }

  std::vector<core::Board::Placement> move = {{{'A', 1}, 6, 9},
                                              {{'T', 1}, 6, 10}};
  const auto board_response = board.ValidateMove(move, lexicon);
  CHECK(board_response.status == core::Board::ResponseStatus::kSuccess);

  std::vector<core::Board::Placement> move2 = {{{'P', 3}, 4, 10}};
  const auto board_response2 = board.ValidateMove(move2, lexicon);
  CHECK(board_response2.status == core::Board::ResponseStatus::kSuccess);
}

// TEST_CASE("CLI Game") {
//   cli::CLIGame cli_game;
//   cli_game.Start();
// }

TEST_CASE("Bot player") {}
