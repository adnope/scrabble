#include "board.hpp"

#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "square.hpp"
#include "tile.hpp"

namespace core {
Board::Board() : is_first_move_(true) {
  const std::string default_board =
      "t..2...t...2..t\n"
      ".d...3...3...d.\n"
      "..d...2.2...d..\n"
      "...d.......d...\n"
      "....d.....d....\n"
      ".3...3...3...3.\n"
      "..2...2.2...2..\n"
      "t.............t\n"
      "..2...2.2...2..\n"
      ".3...3...3...3.\n"
      "....d.....d....\n"
      "...d.......d...\n"
      "..d...2.2...d..\n"
      ".d...3...3...d.\n"
      "t..2...t...2..t";

  using Symbol = Square::Symbol;
  using Multiplier = Square::Multiplier;

  std::istringstream board_stream(default_board);

  std::string line;
  int row = 0;
  while (std::getline(board_stream, line)) {
    for (int col = 0; col < static_cast<int>(line.size()); ++col) {
      if (row == kStartPosRow && col == kStartPosColumn) {
        Square square(row, col, Symbol::kStart, Multiplier::kDoubleWord);
        board_grid_.at(row).at(col) = square;
        continue;
      }
      char c = line[col];
      switch (c) {
        case '.': {
          Square square(row, col, Symbol::kNormal, Multiplier::kNormal);
          board_grid_.at(row).at(col) = square;
          break;
        }
        case '2': {
          Square square(row, col, Symbol::kDoubleLetter,
                        Multiplier::kDoubleLetter);
          board_grid_.at(row).at(col) = square;
          break;
        }
        case '3': {
          Square square(row, col, Symbol::kTripleLetter,
                        Multiplier::kTripleLetter);
          board_grid_.at(row).at(col) = square;
          break;
        }
        case 'd': {
          Square square(row, col, Symbol::kDoubleWord, Multiplier::kDoubleWord);
          board_grid_.at(row).at(col) = square;
          break;
        }
        case 't': {
          Square square(row, col, Symbol::kTripleWord, Multiplier::kTripleWord);
          board_grid_.at(row).at(col) = square;
          break;
        }
        default:
          std::cout << "Invalid board character\n";
      }
    }
    ++row;
  }
}

bool Board::PlaceTile(const Tile& tile, const int row, const int col) {
  if (!IsOccupied(row, col)) {
    board_grid_.at(row).at(col).PlaceTile(tile);
    return true;
  }
  std::cout << "Failed to place tile at: " << row << " " << col << '\n';
  return false;
}

// Return the points of the word to parameter 'points'
Board::Word Board::GetWordFromPos(int row, int col, const bool horizontal,
                                  const std::vector<Placement>& move) {
  // Go backward to find start
  while (row >= 0 && col >= 0 && board_grid_.at(row).at(col).IsOccupied()) {
    if (horizontal) {
      --col;
    } else {
      --row;
    }
  }
  if (horizontal) {
    ++col;
  } else {
    ++row;
  }

  Word word;
  std::cout << "=====New word created\n";
  // Iterate through word from the start of the word
  while (row < kHeight && col < kWidth &&
         board_grid_.at(row).at(col).IsOccupied()) {
    std::cout << "found letter: " << board_grid_.at(row).at(col).tile_letter()
              << '\n';
    // Add the letter of the tile at square {row, col} to the word's content
    word.content += board_grid_.at(row).at(col).tile_letter();

    // Get the multiplier of the square at {row, col} and its tile_points
    const auto multiplier = board_grid_.at(row).at(col).multiplier();
    int tile_points = board_grid_.at(row).at(col).tile_points();

    // Check if the tile at {row, col} is a newly placed tile
    bool is_newly_placed_tile = false;
    for (const auto& placement : move) {
      if (row == placement.row && col == placement.col) {
        is_newly_placed_tile = true;
        break;
      }
    }

    // Only apply premium square's boost to newly placed tiles
    if (is_newly_placed_tile) {
      switch (multiplier) {
        case Square::Multiplier::kNormal: {
          break;
        }
        case Square::Multiplier::kDoubleLetter: {
          tile_points *= 2;
          break;
        }
        case Square::Multiplier::kTripleLetter: {
          tile_points *= 3;
          break;
        }
        // Add word-boost to the word
        case Square::Multiplier::kDoubleWord: {
          word.multiplier = 2;
          break;
        }
        case Square::Multiplier::kTripleWord: {
          word.multiplier = 3;
          break;
        }
      }
    }

    word.points += tile_points;
    std::cout << "Added " << tile_points << " points to word\n";

    if (horizontal) {
      ++col;
    } else {
      ++row;
    }
  }

  word.ApplyMultiplier();
  std::cout << "Word points: " << word.points
            << "\n=====Word calculation done\n";
  return word;
}

std::vector<Board::Word> Board::GetWordsFromMove(
    const std::vector<Placement>& move, const bool horizontal) {
  auto backup_board = board_grid_;

  // Temporarily place the move to get words
  for (const auto& [tile, row, col] : move) {
    PlaceTile(tile, row, col);
  }

  std::vector<Word> words;

  Word main_word = GetWordFromPos(move[0].row, move[0].col, horizontal, move);
  if (main_word.content.size() > 1) {
    words.emplace_back(main_word);
  }

  // Cross words
  for (const auto& p : move) {
    Word cross_word = GetWordFromPos(p.row, p.col, !horizontal, move);
    if (cross_word.content.size() > 1) {
      words.emplace_back(cross_word);
    }
  }

  // Reverse the changes
  board_grid_ = backup_board;
  return words;
}

bool Board::IsMoveOccupied(const std::vector<Placement>& move) const {
  for (const auto& [tile, row, col] : move) {
    if (IsOccupied(row, col)) {
      std::cout << "Move at: " << row << " " << col << " occupied\n";
      return true;
    }
  }
  return false;
}

int Board::IsAligned(const std::vector<Placement>& move) {
  int anchor_row = move[0].row;
  int anchor_col = move[0].col;
  bool horizontal = true;
  for (const auto& [tile, row, col] : move) {
    if (row != anchor_row) {
      horizontal = false;
      break;
    }
  }
  if (!horizontal) {
    for (const auto& [tile, row, col] : move) {
      if (col != anchor_col) {
        std::cout << "Move placements not aligned\n";
        return -1;
      }
    }
  }

  return static_cast<int>(horizontal);
}

bool Board::AreInDictionary(const std::vector<std::string>& words,
                            const Dictionary& dictionary) {
  for (const auto& word : words) {
    if (!dictionary.Contains(word)) {
      std::cout << "Invalid word: " << word << '\n';
      return false;
    }
  }
  return true;
}

int Board::PlaceMoveAndGetScore(const std::vector<Placement>& move,
                                const Dictionary& dictionary) {
  // Check if any placement is performed on occupied square
  if (IsMoveOccupied(move)) {
    return -1;
  }

  // Checking placements alignment
  int horizontal = IsAligned(move);
  if (horizontal == -1) {
    return horizontal;
  }

  // Checking words validity
  int score = 0;
  const auto words = GetWordsFromMove(move, static_cast<bool>(horizontal));
  std::vector<std::string> word_list;
  word_list.reserve(words.size());
  for (const auto& word : words) {
    word_list.push_back(word.content);
  }
  if (!AreInDictionary(word_list, dictionary)) {
    return -1;
  }
  std::cout << "Words formed: ";
  for (const auto& word : words) {
    std::cout << word.content << word.points << ' ';
  }
  std::cout << '\n';

  // Place the move after checking errors
  for (const auto& [tile, row, col] : move) {
    PlaceTile(tile, row, col);
  }

  return score;
}

std::string Board::GetDisplayFormat() {
  std::string result;
  for (const auto& row : board_grid_) {
    for (const Square& square : row) {
      if (square.IsOccupied()) {
        result += std::string{square.tile_letter()} + "  ";
      } else {
        Square::Symbol symbol = square.symbol();
        switch (symbol) {
          case Square::Symbol::kNormal:
            result += ".  ";
            break;
          case Square::Symbol::kStart:
            result += "*  ";
            break;
          case Square::Symbol::kDoubleLetter:
            result += "DL ";
            break;
          case Square::Symbol::kDoubleWord:
            result += "DW ";
            break;
          case Square::Symbol::kTripleLetter:
            result += "TL ";
            break;
          case Square::Symbol::kTripleWord:
            result += "TW ";
            break;
        }
      }
    }
    result += '\n';
  }
  return result;
}
}  // namespace core