#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <iostream>
#include <string>

#include "core/board.hpp"
#include "core/dictionary.hpp"
#include "core/lexicon.hpp"
#include "core/square.hpp"
#include "core/word.hpp"
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
    case core::Board::ResponseStatus::kNotAdjacent: {
      std::cout << "Status: MOVE NOT ADJACENT TO EXISTING TILES\n";
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
    case core::Board::ResponseStatus::kNotInStartingSquare: {
      std::cout << "Status: NOT IN STARTING SQUARE\n";
      break;
    }
  }
}

TEST_CASE("Board test") {
  spdlog::info("Board test");
  core::Board board;
  core::Lexicon lexicon;
  lexicon.PreloadDictionary(core::Dictionary::CSW);

  std::vector<core::Board::Placement> initial_state_move = {
      {{'V', 4}, 5, 7},  {{'I', 1}, 5, 8}, {{'S', 1}, 5, 9}, {{'I', 1}, 5, 10},
      {{'T', 1}, 5, 11}, {{'T', 1}, 7, 4}, {{'A', 1}, 7, 5}, {{'N', 1}, 7, 6},
      {{'N', 1}, 7, 7},  {{'E', 1}, 7, 8}, {{'D', 2}, 7, 9}, {{'P', 3}, 2, 8},
      {{'R', 1}, 3, 8},  {{'A', 1}, 4, 8}, {{'S', 1}, 6, 8}};
  for (const auto& placement : initial_state_move) {
    board.PlaceTile(placement.tile, placement.row, placement.col);
  }

  std::vector<core::Board::Placement> move = {{{'A', 1}, 6, 9},
                                              {{'T', 1}, 6, 10}};
  const auto board_response = board.ValidateMove(move, lexicon);
  for (const core::Word& word : board_response.words) {
    word.PrintContent();
    std::cout << "-" << word.points() << "points" << " ";
  }
  std::cout << "\n";
  CHECK(board_response.status == core::Board::ResponseStatus::kSuccess);

  std::vector<core::Board::Placement> move2 = {{{'P', 3}, 4, 10}};
  const auto board_response2 = board.ValidateMove(move2, lexicon);
  for (const core::Word& word : board_response2.words) {
    word.PrintContent();
    std::cout << "-" << word.points() << "points" << " ";
  }
  std::cout << "\n";
  CHECK(board_response2.status == core::Board::ResponseStatus::kSuccess);
  // std::cout << board.GetDisplayFormat();
}

// TEST_CASE("Bot player find best move") {
//   std::cout<<"Bot test"<<std::endl;
//   core::Board board;
//   core::Lexicon lexicon;
//   lexicon.PreLoadDictionary(core::Dictionary::DictionaryType::CSW);

//   std::vector<core::Board::Placement> initial_state_move = {
//       {{'V', 4}, 5, 7},  {{'I', 1}, 5, 8}, {{'S', 1}, 5, 9}, {{'I', 1}, 5,
//       10},
//       {{'T', 1}, 5, 11}, {{'T', 1}, 7, 4}, {{'A', 1}, 7, 5}, {{'N', 1}, 7,
//       6},
//       {{'N', 1}, 7, 7},  {{'E', 1}, 7, 8}, {{'D', 2}, 7, 9}, {{'P', 3}, 2,
//       8},
//       {{'R', 1}, 3, 8},  {{'A', 1}, 4, 8}, {{'S', 1}, 6, 8}};
//   for (const auto& placement : initial_state_move) {
//     board.PlaceTile(placement.tile, placement.row, placement.col);
//   }

//   core::Bot bot;
//   SUBCASE("No blank tile: A, T, E, S, P, R, O") {
//     std::vector<core::Tile> rack = {core::Tile('A', 1), core::Tile('T', 1),
//                                     core::Tile('E', 1), core::Tile('S', 1),
//                                     core::Tile('P', 3), core::Tile('R', 1),
//                                     core::Tile('O', 1)};
//     auto best_move = core::Bot::FindBestMove(rack, board, lexicon);

//     //Best move empty ??
//     if(!best_move.empty()){std::cout<<"Best move should not be empty";}

//     auto response = board.ValidateMove(best_move, lexicon);
//     CHECK_MESSAGE(response.status == core::Board::ResponseStatus::kSuccess,
//                   "Best move should be valid");

//     // Kiểm tra từ được tạo: "OPERATE"
//     bool found_OPERATE = false;
//     for (const auto& word : response.words) {
//       std::string word_str = word.AsString();
//       if (word_str == "OPERATE") {
//         found_OPERATE = true;
//       }
//     }
//     CHECK_MESSAGE(found_OPERATE, "Best move should form word 'OPERATE'");

//     // Kiểm tra điểm số: OPERATE (O=1, P=3*3=9, E=1, R=1, A=1, T=1, E=1) = 15
//     CHECK_MESSAGE(response.move_points == 15,
//                   "Best move should score 15 points");

//     // Kiểm tra vị trí: O(4,10), P(4,11), E(4,12), R(4,13), A(4,14), T(4,15),
//     // E(4,16)
//     bool found_positions[7] = {false};
//     for (const auto& placement : best_move) {
//       if (placement.row == 4 && placement.col == 10 &&
//           placement.tile.letter() == 'O' && placement.tile.points() == 1) {
//         found_positions[0] = true;
//       }
//       if (placement.row == 4 && placement.col == 11 &&
//           placement.tile.letter() == 'P' && placement.tile.points() == 3) {
//         found_positions[1] = true;
//       }
//       if (placement.row == 4 && placement.col == 12 &&
//           placement.tile.letter() == 'E' && placement.tile.points() == 1) {
//         found_positions[2] = true;
//       }
//       if (placement.row == 4 && placement.col == 13 &&
//           placement.tile.letter() == 'R' && placement.tile.points() == 1) {
//         found_positions[3] = true;
//       }
//       if (placement.row == 4 && placement.col == 14 &&
//           placement.tile.letter() == 'A' && placement.tile.points() == 1) {
//         found_positions[4] = true;
//       }
//       if (placement.row == 4 && placement.col == 15 &&
//           placement.tile.letter() == 'T' && placement.tile.points() == 1) {
//         found_positions[5] = true;
//       }
//       if (placement.row == 4 && placement.col == 16 &&
//           placement.tile.letter() == 'E' && placement.tile.points() == 1) {
//         found_positions[6] = true;
//       }
//     }
//     for (int i = 0; i < 7; ++i) {
//       CHECK_MESSAGE(found_positions[i],
//                     "Best move should place correct tile at position (4, "
//                         << 10 + i << ")");
//     }

//     for (const auto& placement : best_move) {
//       board.PlaceTile(placement.tile, placement.row, placement.col);
//     }
//     INFO("Board state after placing tiles (No blank tile):");
//     PrintBoardResponse(response);

//   }

//   SUBCASE("One blank tile: ?, A, T, E, S, P, R") {
//     // core::Tiles: {?, A, T, E, S, P, R}
//     std::vector<core::Tile> rack = {core::Tile('?', 0), core::Tile('A', 1),
//                                     core::Tile('T', 1), core::Tile('E', 1),
//                                     core::Tile('S', 1), core::Tile('P', 3),
//                                     core::Tile('R', 1)};
//     auto best_move = bot.FindBestMove(rack, board, lexicon);

//     // Kiểm tra nước đi không rỗng
//     CHECK_MESSAGE(!best_move.empty(), "Best move should not be empty");

//     // Kiểm tra tính hợp lệ
//     auto response = board.ValidateMove(best_move, lexicon);
//     CHECK_MESSAGE(response.status == core::Board::ResponseStatus::kSuccess,
//                   "Best move should be valid");

//     // Kiểm tra từ được tạo: "OPERATE"
//     bool found_OPERATE = false;
//     for (const auto& word : response.words) {
//       std::string word_str = word.AsString();
//       if (word_str == "OPERATE") found_OPERATE = true;
//     }
//     CHECK_MESSAGE(found_OPERATE, "Best move should form word 'OPERATE'");

//     // Kiểm tra điểm số: OPERATE (O=0 from ?, P=3*3=9, E=1, R=1, A=1, T=1,
//     E=1)
//     // = 14
//     CHECK_MESSAGE(response.move_points == 14,
//                   "Best move should score 14 points");

//     // Kiểm tra vị trí: ?(4,10) as O, P(4,11), E(4,12), R(4,13), A(4,14),
//     // T(4,15), E(4,16)
//     bool found_positions[7] = {false};
//     for (const auto& placement : best_move) {
//       if (placement.row == 4 && placement.col == 10 &&
//           placement.tile.letter() == '?' && placement.tile.points() == 0) {
//         found_positions[0] = true;
//       }
//       if (placement.row == 4 && placement.col == 11 &&
//           placement.tile.letter() == 'P' && placement.tile.points() == 3) {
//         found_positions[1] = true;
//       }
//       if (placement.row == 4 && placement.col == 12 &&
//           placement.tile.letter() == 'E' && placement.tile.points() == 1) {
//         found_positions[2] = true;
//       }
//       if (placement.row == 4 && placement.col == 13 &&
//           placement.tile.letter() == 'R' && placement.tile.points() == 1) {
//         found_positions[3] = true;
//       }
//       if (placement.row == 4 && placement.col == 14 &&
//           placement.tile.letter() == 'A' && placement.tile.points() == 1) {
//         found_positions[4] = true;
//       }
//       if (placement.row == 4 && placement.col == 15 &&
//           placement.tile.letter() == 'T' && placement.tile.points() == 1) {
//         found_positions[5] = true;
//       }
//       if (placement.row == 4 && placement.col == 16 &&
//           placement.tile.letter() == 'E' && placement.tile.points() == 1) {
//         found_positions[6] = true;
//       }
//     }
//     for (int i = 0; i < 7; ++i) {
//       CHECK_MESSAGE(found_positions[i],
//                     "Best move should place correct tile at position (4, "
//                         << 10 + i << ")");
//     }
//     for (const auto& placement : best_move) {
//       board.PlaceTile(placement.tile, placement.row, placement.col);
//     }
//     INFO("Board state after placing tiles (No blank tile):");
//     PrintBoardResponse(response);
//   }

//   SUBCASE("Two blank tiles: ?, ?, A, T, E, S, P") {
//     std::vector<core::Tile> rack = {core::Tile('?', 0), core::Tile('?', 0),
//                                     core::Tile('A', 1), core::Tile('T', 1),
//                                     core::Tile('E', 1), core::Tile('S', 1),
//                                     core::Tile('P', 3)};
//     auto best_move = bot.FindBestMove(rack, board, lexicon);

//     CHECK_MESSAGE(!best_move.empty(), "Best move should not be empty");

//     // Kiểm tra tính hợp lệ
//     auto response = board.ValidateMove(best_move, lexicon);
//     CHECK_MESSAGE(response.status == core::Board::ResponseStatus::kSuccess,
//                   "Best move should be valid");

//     // Kiểm tra từ được tạo: "OPERATE"
//     bool found_OPERATE = false;
//     for (const auto& word : response.words) {
//       std::string word_str = word.AsString();
//       if (word_str == "OPERATE") found_OPERATE = true;
//     }
//     CHECK_MESSAGE(found_OPERATE, "Best move should form word 'OPERATE'");

//     // Kiểm tra điểm số: OPERATE (O=0 from ?, P=3*3=9, E=1, R=0 from ?, A=1,
//     // T=1, E=1) = 13
//     CHECK_MESSAGE(response.move_points == 13,
//                   "Best move should score 13 points");

//     // Kiểm tra vị trí: ?(4,10) as O, P(4,11), E(4,12), ?(4,13) as R,
//     A(4,14),
//     // T(4,15), E(4,16)
//     bool found_positions[7] = {false};
//     bool found_blank_1 = false;
//     bool found_blank_2 = false;
//     for (const auto& placement : best_move) {
//       if (placement.row == 4 && placement.col == 10 &&
//           placement.tile.letter() == '?' && placement.tile.points() == 0) {
//         found_positions[0] = true;
//         found_blank_1 = true;
//       }
//       if (placement.row == 4 && placement.col == 11 &&
//           placement.tile.letter() == 'P' && placement.tile.points() == 3) {
//         found_positions[1] = true;
//       }
//       if (placement.row == 4 && placement.col == 12 &&
//           placement.tile.letter() == 'E' && placement.tile.points() == 1) {
//         found_positions[2] = true;
//       }
//       if (placement.row == 4 && placement.col == 13 &&
//           placement.tile.letter() == '?' && placement.tile.points() == 0 &&
//           !found_blank_1) {
//         found_positions[3] = true;
//         found_blank_2 = true;
//       }
//       if (placement.row == 4 && placement.col == 14 &&
//           placement.tile.letter() == 'A' && placement.tile.points() == 1) {
//         found_positions[4] = true;
//       }
//       if (placement.row == 4 && placement.col == 15 &&
//           placement.tile.letter() == 'T' && placement.tile.points() == 1) {
//         found_positions[5] = true;
//       }
//       if (placement.row == 4 && placement.col == 16 &&
//           placement.tile.letter() == 'E' && placement.tile.points() == 1) {
//         found_positions[6] = true;
//       }
//     }
//     for (int i = 0; i < 7; ++i) {
//       CHECK_MESSAGE(found_positions[i],
//                     "Best move should place correct tile at position (4, "
//                         << 10 + i << ")");
//     }
//     CHECK_MESSAGE(found_blank_1,
//                   "Best move should use first blank tile at (4, 10)");
//     CHECK_MESSAGE(found_blank_2,
//                   "Best move should use second blank tile at (4, 13)");

//     for (const auto& placement : best_move) {
//     board.PlaceTile(placement.tile, placement.row, placement.col);
//     }
//     INFO("Board state after placing tiles (No blank tile):");
//     PrintBoardResponse(response);
//   }
// }
