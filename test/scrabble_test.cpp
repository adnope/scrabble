#include "core/word.hpp"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <iostream>
#include <string>

#include "cli/game_cli.hpp"
#include "core/bag.hpp"
#include "core/board.hpp"
#include "core/bot.hpp"
#include "core/dictionary.hpp"
#include "core/lexicon.hpp"
#include "core/player.hpp"
#include "core/square.hpp"
#include "core/tile.hpp"
#include "doctest/doctest.h"
#include "spdlog/spdlog.h"

// TEST_CASE("Lexicon contains test") {
//   core::Lexicon lexicon;
//   lexicon.AddWord("cat");
//   lexicon.AddWord("car");
//   lexicon.AddWord("dog");

//   SUBCASE("Lexicon size") { CHECK(lexicon.size() == 8); }

//   SUBCASE("Lexicon contains words") {
//     CHECK(lexicon.Contains("cat") == true);
//     CHECK(lexicon.Contains("car") == true);
//     CHECK(lexicon.Contains("dog") == true);
//   }

//   SUBCASE("Lexicon contains prefixes") {
//     CHECK(lexicon.ContainsPrefix("ca") == true);
//     CHECK(lexicon.ContainsPrefix("do") == true);
//     CHECK(lexicon.ContainsPrefix("fi") == false);
//     CHECK(lexicon.ContainsPrefix("bi") == false);
//   }
// }



// TEST_CASE("Lexicon preload test") {
//   core::Lexicon lexicon;

//   SUBCASE("Random word check with CSW dictionary") {
//     lexicon.PreloadDictionary(core::Dictionary::DictionaryType::CSW);
//     CHECK(lexicon.ContainsPrefix("tnemngissa+") == true);
//   }

//   SUBCASE("Random word check with TWL dictionary") {
//     lexicon.PreloadDictionary(core::Dictionary::DictionaryType::TWL);
//     CHECK(lexicon.ContainsPrefix("tnemngissa+") == true);
//   }
// }

// TEST_CASE("Player test") {
//   using core::Bag;
//   using core::Tile;

//   Bag bag;
//   core::Player player("test_player_1", 0);
//   std::vector<Tile> initial_tiles = {{'A', 1}, {'B', 3}, {'C', 3}, {'D', 2},
//                                      {'E', 1}, {'F', 4}, {'G', 2}};
//   player.AddTiles(initial_tiles);
//   spdlog::info("New player created");
//   player.PrintDeck();
//   std::cout << "Subcase: ";

//   SUBCASE("Constructor") {
//     std::cout << "[Player constructor]\n";

//     CHECK(player.name() == "test_player_1");
//     CHECK(player.score() == 0);
//     CHECK(player.current_deck_size() == 7);
//   }

//   SUBCASE("Use tiles and draw tiles") {
//     std::cout << "[Player use tiles and draw tiles]\n";

//     player.UseTile(1);
//     player.UseTile(4);
//     player.UseTile(5);
//     std::cout << "Deck before: \n";
//     player.PrintDeck();
//     CHECK(player.current_deck_size() == 4);

//     player.DrawNewTiles(bag);
//     std::cout << "Deck before: \n";
//     player.PrintDeck();
//     CHECK(player.current_deck_size() == 7);
//   }

//   SUBCASE("Swapping") {
//     std::cout << "[Player swap]\n";
//     std::cout << "Deck before: \n";
//     player.PrintDeck();

//     player.PerformSwap(bag, {1, 2, 3});

//     std::cout << "Deck after: \n";
//     player.PrintDeck();

//     CHECK(player.current_deck_size() == 7);
//   }
// }

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

TEST_CASE("Bot test") {
  spdlog::info("Bot test");
  core::Board board;
  core::Lexicon lexicon;
  core::Bag bag;
  core::Bot bot("TestBot");
  lexicon.PreloadDictionary(core::Dictionary::CSW);

  // Dat ban dau bang
  std::vector<core::Board::Placement> initial_state_move = {
      {{'V', 4}, 5, 7},  {{'I', 1}, 5, 8}, {{'S', 1}, 5, 9}, {{'I', 1}, 5, 10},
      {{'T', 1}, 5, 11}, {{'T', 1}, 7, 4}, {{'A', 1}, 7, 5}, {{'N', 1}, 7, 6},
      {{'N', 1}, 7, 7},  {{'E', 1}, 7, 8}, {{'D', 2}, 7, 9}, {{'P', 3}, 2, 8},
      {{'R', 1}, 3, 8},  {{'A', 1}, 4, 8}, {{'S', 1}, 6, 8}};
  for (const auto& placement : initial_state_move) {
    board.PlaceTile(placement.tile, placement.row, placement.col);
  }

  // Them cac o chu de bot co the su dung
  std::vector<core::Tile> initial_tiles = {{'A', 1}, {'T', 1}};
  bot.AddTiles(initial_tiles);

  // Chuyen doi move sang dang Bot::Move
  core::Bot::Move bot_move = {
      {0, 6, 9},  // A tai vi tri (6, 9)
      {1, 6, 10}  // T tai vi tri (6, 10)
  };
  CHECK(bot.IsValidMove(bot_move, board) == true);

  // Goi FindBestMove de kiem tra
  core::Bot::Move best_move = bot.FindBestMove(board, lexicon, bag);
  if (!best_move.empty()) {
    const auto board_response = board.ValidateMove(core::Bot::ConvertToBoardMove(bot, best_move), lexicon);
    for (const core::Word& word : board_response.words) {
      word.PrintContent();
      std::cout << "-" << word.points() << "points" << " ";
    }
    std::cout << "\n";
    CHECK(board_response.status == core::Board::ResponseStatus::kSuccess);
  } else {
    std::cout << "No valid move found.\n";
  }

  // Kiem tra move2 voi Bot
  core::Bot::Move bot_move2 = {{0, 4, 10}}; // Su dung o A tai (4, 10), can cap nhat tile neu khac
  if (bot.IsValidMove(bot_move2, board)) {
    const auto board_response2 = board.ValidateMove(bot.ConvertToBoardMove(bot, bot_move2), lexicon);
    for (const core::Word& word : board_response2.words) {
      word.PrintContent();
      std::cout << "-" << word.points() << "points" << " ";
    }
    std::cout << "\n";
    CHECK(board_response2.status == core::Board::ResponseStatus::kSuccess);
  }

  std::cout << board.GetDisplayFormat();
}

// TEST_CASE("Bot FindBestMove with Simulated Board") {
//   core::Bot bot("TestBot");
//   core::Bag bag;
//   core::Board board;
//   core::Lexicon lexicon;
//   lexicon.PreloadDictionary(core::Dictionary::CSW);

//   // Helper function để kiểm tra ô premium từ display format
//   auto isPremiumSquare = [&board](int row, int col) {
//     std::string display = board.GetDisplayFormat();
//     int line_length = (core::Board::kWidth * 3) + 1;  // Mỗi ô 3 ký tự +
//     newline int pos = (row * line_length) + (col * 3);

//     if (pos >= 0 && pos + 2 < display.size()) {
//       std::string square = display.substr(pos, 2);
//       return (square == "DW" || square == "TW" || square == "DL" ||
//               square == "TL");
//     }
//     return false;
//   };

//   // Helper function để chuyển đổi move
//   auto convertToBoardMove = [&bot](const core::Player::Move& player_move) {
//     core::Board::Move board_move;
//     const auto& deck = bot.deck();
//     for (const auto& placement : player_move) {
//       if (placement.tile_index >= 0 &&
//           placement.tile_index < static_cast<int>(deck.size())) {
//         board_move.push_back(
//             {deck.at(placement.tile_index), placement.row, placement.col});
//       }
//     }
//     return board_move;
//   };

//   // Thiết lập bảng giả lập với try-catch
//   try {
//     std::vector<core::Board::Placement> initial_placements = {
//         {{'H', 4}, 7, 7},  {{'E', 1}, 7, 8},  {{'L', 1}, 7, 9},
//         {{'L', 1}, 7, 10}, {{'O', 1}, 7, 11},  // "HELLO" ngang
//         {{'W', 4}, 5, 7},  {{'O', 1}, 6, 7},   // "WORD" dọc
//         {{'R', 1}, 7, 7},  {{'D', 2}, 8, 7}};

//     for (const auto& placement : initial_placements) {
//       if (placement.row >= 0 && placement.row < core::Board::kHeight &&
//           placement.col >= 0 && placement.col < core::Board::kWidth) {
//         if (!board.PlaceTile(placement.tile, placement.row, placement.col)) {
//           std::cerr << "Failed to place tile at: " << placement.row << ","
//                     << placement.col << "\n";
//         }
//       }
//     }
//   } catch (const std::exception& e) {
//     FAIL("Board setup failed: ", e.what());
//   }

//   SUBCASE("Horizontal Move Connecting to Existing Word") {
//     try {
//       std::vector<core::Tile> tiles = {{'S', 1}, {'H', 4}, {'E', 1}, {'P',
//       3}}; bot.AddTiles(tiles);

//       auto best_move = bot.FindBestMove(board, lexicon, bag);
//       REQUIRE_MESSAGE(best_move.size() > 0,
//                       "Bot failed to find any valid move");

//       core::Board::Move board_move = convertToBoardMove(best_move);

//       // Validate move coordinates
//       for (const auto& placement : board_move) {
//         INFO("Checking placement at row: " << placement.row
//                                            << ", col: " << placement.col);
//         CHECK(placement.row >= 0);
//         CHECK(placement.row < core::Board::kHeight);
//         CHECK(placement.col >= 0);
//         CHECK(placement.col < core::Board::kWidth);
//       }

//       auto response = board.ValidateMove(board_move, lexicon);
//       CHECK_MESSAGE(response.status == core::Board::ResponseStatus::kSuccess,
//                     "Move validation failed");

//       // Debug output
//       std::cout << "Generated move:\n";
//       for (const auto& p : board_move) {
//         std::cout << p.tile.letter() << " @ (" << p.row << "," << p.col
//                   << ")\n";
//       }
//       PrintBoardResponse(response);

//     } catch (const std::exception& e) {
//       FAIL("Test threw exception: ", e.what());
//     }
//   }

//   SUBCASE("Premium Square Utilization") {
//     try {
//       std::vector<core::Tile> tiles = {
//           {'Q', 10}, {'U', 1}, {'I', 1}, {'Z', 10}};
//       bot.AddTiles(tiles);

//       auto best_move = bot.FindBestMove(board, lexicon, bag);
//       REQUIRE(best_move.size() > 0);

//       core::Board::Move board_move = convertToBoardMove(best_move);
//       auto response = board.ValidateMove(board_move, lexicon);
//       REQUIRE(response.status == core::Board::ResponseStatus::kSuccess);

//       // Check premium square usage
//       bool uses_premium = false;
//       for (const auto& placement : board_move) {
//         if (isPremiumSquare(placement.row, placement.col)) {
//           uses_premium = true;
//           break;
//         }
//       }
//       CHECK_MESSAGE(uses_premium, "Bot didn't use any premium squares");

//     } catch (const std::exception& e) {
//       FAIL("Premium square test failed: ", e.what());
//     }
//   }
// }

// //OK
// TEST_CASE("Bot::IsAnchor function") {
//   core::Board board;
//   core::Bot bot("TestBot");

//   // Setup board with some tiles
//   board.PlaceTile({'A', 1}, 7, 7);
//   board.PlaceTile({'B', 3}, 7, 8);

//   SUBCASE("Occupied square is not anchor") {
//     CHECK(bot.IsAnchor(7, 7, board) == false);
//   }

//   SUBCASE("Empty square adjacent to occupied is anchor") {
//     CHECK(bot.IsAnchor(7, 6, board) == true);  // Left of A
//     CHECK(bot.IsAnchor(6, 7, board) == true);  // Above A
//   }

//   SUBCASE("Empty square not adjacent is not anchor") {
//     CHECK(bot.IsAnchor(0, 0, board) == false);
//   }

//   SUBCASE("Out of bounds is not anchor") {
//     CHECK(bot.IsAnchor(-1, 7, board) == false);
//     CHECK(bot.IsAnchor(15, 7, board) == false);
//   }
// }

// TEST_CASE("Bot::FindBestMove function") {
//   core::Board board;
//   core::Bag bag;
//   core::Lexicon lexicon;
//   core::Bot bot("TestBot");

//   lexicon.AddWord("hello");
//   lexicon.AddWord("word");
//   lexicon.AddWord("cat");

//   SUBCASE("First move with good tiles") {
//     bot.AddTiles({{'H', 4}, {'E', 1}, {'L', 1}, {'L', 1}, {'O', 1}});
//     auto move = bot.FindBestMove(board, lexicon, bag);
//     CHECK(move.size() > 0);

//     // Verify it places at least one tile
//     bool places_tile = false;
//     for (const auto& placement : move) {
//       if (placement.tile_index != -1) {
//         places_tile = true;
//         break;
//       }
//     }
//     CHECK(places_tile);
//   }

//   SUBCASE("No valid moves should swap or pass") {
//     bot.AddTiles({{'X', 8}, {'Z', 10}, {'Q', 10}});  // Hard to play letters
//     auto move = bot.FindBestMove(board, lexicon, bag);

//     // Either swap or pass
//     bool valid = (move.empty() ||                           // Pass
//                   (move.size() > 0 && move[0].row == -1));  // Swap
//     CHECK(valid);
//   }

//   SUBCASE("Connects to existing words") {
//     // Setup board
//     board.PlaceTile({'H', 4}, 7, 7);
//     board.PlaceTile({'E', 1}, 7, 8);

//     bot.AddTiles({{'L', 1}, {'L', 1}, {'O', 1}});
//     auto move = bot.FindBestMove(board, lexicon, bag);
//     CHECK(move.size() >= 3);  // Should complete "HELLO"
//   }
// }

// TEST_CASE("Bot::GenerateAllMoves function") {
//   core::Board board;
//   core::Lexicon lexicon;
//   core::Bot bot("TestBot");

//   lexicon.AddWord("hello");
//   lexicon.AddWord("cat");

//   SUBCASE("First move generation") {
//     bot.AddTiles({{'H', 4}, {'E', 1}, {'L', 1}, {'L', 1}, {'O', 1}});
//     auto moves = bot.GenerateAllMoves(board, lexicon);
//     CHECK(moves.size() > 0);

//     // Should include "HELLO" placement
//     bool found_hello = false;
//     for (const auto& move : moves) {
//       if (move.size() == 5) found_hello = true;
//     }
//     CHECK(found_hello);
//   }

//   SUBCASE("Anchor move generation") {
//     // Setup board with anchor point
//     board.PlaceTile({'C', 3}, 7, 7);
//     board.PlaceTile({'A', 1}, 7, 8);

//     bot.AddTiles({{'T', 1}, {'E', 1}, {'L', 1}});
//     auto moves = bot.GenerateAllMoves(board, lexicon);
//     CHECK(moves.size() > 0);

//     // Should include moves connecting to "CA"
//     bool connects = false;
//     for (const auto& move : moves) {
//       for (const auto& placement : move) {
//         if (placement.row == 7 && placement.col == 9) {
//           connects = true;
//           break;
//         }
//       }
//     }
//     CHECK(connects);
//   }
// }

//   SUBCASE("Invalid first move - not covering center") {
//     core::Board board;
//     core::Board::Move invalid_move = {
//       {{'H', 4}, 6, 7}, // Không qua ô trung tâm
//       {{'E', 1}, 7, 7}  // Dù có ô trung tâm nhưng không phải là điểm bắt đầu
//     };
//     auto response = board.ValidateMove(invalid_move, lexicon);
//     CHECK(response.status == core::Board::ResponseStatus::kFailure);
//   }

//   SUBCASE("Valid move - extending existing word horizontally") {
//     core::Board board;
//     // Tạo từ "HE" ngang
//     board.PlaceTile({'H', 4}, 7, 7);
//     board.PlaceTile({'E', 1}, 7, 8);
    
//     // Mở rộng thành "HELL"
//     core::Board::Move move = {
//       {{'L', 1}, 7, 9},
//       {{'L', 1}, 7, 10}
//     };
//     auto response = board.ValidateMove(move, lexicon);
//     CHECK(response.status == core::Board::ResponseStatus::kSuccess);
//     CHECK(response.words.size() >= 1);
//   }

//   SUBCASE("Valid move - creating cross words") {
//     core::Board board;
//     // Tạo từ "HE" ngang
//     board.PlaceTile({'H', 4}, 7, 7);
//     board.PlaceTile({'E', 1}, 7, 8);
    
//     // Đặt tile tạo từ "WE" dọc
//     core::Board::Move move = {
//       {{'W', 4}, 8, 7},
//       {{'E', 1}, 9, 7}
//     };
//     auto response = board.ValidateMove(move, lexicon);
//     CHECK(response.status == core::Board::ResponseStatus::kSuccess);
//     CHECK(response.words.size() == 2); // Cả từ ngang và dọc
//   }

//   SUBCASE("Invalid - tiles not aligned") {
//     core::Board board;
//     core::Board::Move move = {
//       {{'H', 4}, 7, 7},
//       {{'E', 1}, 8, 8} // Không thẳng hàng
//     };
//     auto response = board.ValidateMove(move, lexicon);
//     CHECK(response.status == core::Board::ResponseStatus::kNotAligned);
//   }

//   SUBCASE("Invalid - overlapping existing tiles") {
//     core::Board board;
//     board.PlaceTile({'H', 4}, 7, 7);
    
//     core::Board::Move move = {
//       {{'X', 8}, 7, 7} // Đè lên ô đã có
//     };
//     auto response = board.ValidateMove(move, lexicon);
//     CHECK(response.status == core::Board::ResponseStatus::kOccupied);
//   }

//   SUBCASE("Invalid - creating invalid words") {
//     core::Board board;
//     core::Board::Move move = {
//       {{'X', 8}, 7, 7},
//       {{'Y', 4}, 7, 8} // Tạo "XY" không hợp lệ
//     };
//     auto response = board.ValidateMove(move, lexicon);
//     CHECK(response.status == core::Board::ResponseStatus::kWordsInvalid);
//   }

//   SUBCASE("Valid - single letter word") {
//     core::Board board;
//     board.PlaceTile({'H', 4}, 7, 7);
    
//     // Từ 1 chữ cái hợp lệ
//     core::Board::Move move = {{{'A', 1}, 8, 7}};
//     auto response = board.ValidateMove(move, lexicon);
//     CHECK(response.status == core::Board::ResponseStatus::kSuccess);
//   }

//   SUBCASE("Valid - premium square multipliers") {
//     core::Board board;
//     // Đặt trên ô Triple Word (0,0)
//     core::Board::Move move = {{{'H', 4}, 0, 0}};
//     auto response = board.ValidateMove(move, lexicon);
//     CHECK(response.status == core::Board::ResponseStatus::kSuccess);
//     CHECK(response.move_points == 12); // 4 điểm * 3
//   }

//   SUBCASE("Invalid - not connected to existing words") {
//     core::Board board;
//     board.PlaceTile({'H', 4}, 7, 7);
    
//     // Đặt ở vị trí không kết nối
//     core::Board::Move move = {{{'E', 1}, 0, 0}};
//     auto response = board.ValidateMove(move, lexicon);
//     CHECK(response.status == core::Board::ResponseStatus::kFailure);
//   }
// }