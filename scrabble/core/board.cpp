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
        Square square(Symbol::kStart, Multiplier::kDoubleWord);
        board_grid_.at(row).at(col) = square;
        continue;
      }
      char c = line[col];
      switch (c) {
        case '.': {
          Square square(Symbol::kNormal, Multiplier::kNormal);
          board_grid_.at(row).at(col) = square;
          break;
        }
        case '2': {
          Square square(Symbol::kDoubleLetter, Multiplier::kDoubleLetter);
          board_grid_.at(row).at(col) = square;
          break;
        }
        case '3': {
          Square square(Symbol::kTripleLetter, Multiplier::kTripleLetter);
          board_grid_.at(row).at(col) = square;
          break;
        }
        case 'd': {
          Square square(Symbol::kDoubleWord, Multiplier::kDoubleWord);
          board_grid_.at(row).at(col) = square;
          break;
        }
        case 't': {
          Square square(Symbol::kTripleWord, Multiplier::kTripleWord);
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
Word Board::GetWordFromPos(int row, int col, const bool horizontal,
                           const Move& move) {
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
  int letter_index = 0;
  // Iterate through word from the start of the word
  while (row < kHeight && col < kWidth &&
         board_grid_.at(row).at(col).IsOccupied()) {
    const Square square = board_grid_.at(row).at(col);
    const char letter = square.tile_letter();
    const auto multiplier = square.multiplier();
    int points = square.tile_points();

    // Add the letter and its multiplier to the word's content
    word.AddToContent(letter, Square::Multiplier::kNormal);

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
          points *= 2;
          break;
        }
        case Square::Multiplier::kTripleLetter: {
          points *= 3;
          break;
        }
        // Add word-boost to the word
        case Square::Multiplier::kDoubleWord: {
          word.AddMultiplier(2);
          break;
        }
        case Square::Multiplier::kTripleWord: {
          word.AddMultiplier(3);
          break;
        }
      }
      word.SetMultiplierAt(letter_index, multiplier);
    }

    word.AddPoints(points);

    if (horizontal) {
      ++col;
    } else {
      ++row;
    }

    ++letter_index;
  }

  word.ApplyMultiplier();
  return word;
}

std::vector<Word> Board::GetWordsFromMove(const Move& move,
                                          const bool horizontal) {
  auto backup_board = board_grid_;

  // Temporarily place the move to get words
  for (const auto& [tile, row, col] : move) {
    PlaceTile(tile, row, col);
  }

  std::vector<Word> words;

  Word main_word = GetWordFromPos(move[0].row, move[0].col, horizontal, move);
  if (main_word.word_length() > 1) {
    words.emplace_back(main_word);
  }

  // Cross words
  for (const auto& p : move) {
    Word cross_word = GetWordFromPos(p.row, p.col, !horizontal, move);
    if (cross_word.word_length() > 1) {
      words.emplace_back(cross_word);
    }
  }

  // Reverse the changes
  board_grid_ = backup_board;
  return words;
}

bool Board::IsMoveOccupied(const Move& move) const {
  for (const auto& [tile, row, col] : move) {
    if (IsOccupied(row, col)) {
      std::cout << "Move at: " << row << " " << col << " occupied\n";
      return true;
    }
  }
  return false;
}

int Board::IsAligned(const Move& move) {
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

bool Board::AreInDictionary(const std::vector<std::string>& words, const Lexicon& lexicon) {
  for (const auto& word : words) {
    if (!lexicon.IsContain(word)) {
      std::cout << "Invalid word: " << word << '\n';
      return false;
    }
  }
  return true;
}

Board::MoveValidationResponse Board::ValidateMove(const Move& move,const Lexicon& lexicon) {
  // Check if any placement is performed on occupied square
  if (IsMoveOccupied(move)) {
    return {{}, 0, ResponseStatus::kOccupied};
  }

  // Checking placements alignment
  int horizontal = IsAligned(move);
  if (horizontal == -2) {
    return {{}, 0, ResponseStatus::kNotAligned};
  }

  // Checking words validity
  const auto words = GetWordsFromMove(move, static_cast<bool>(horizontal));
  std::vector<std::string> word_list;
  word_list.reserve(words.size());
  for (const auto& word : words) {
    word_list.push_back(word.AsString());
  }
  if (!AreInDictionary(word_list, lexicon)) {
    return {words, 0, ResponseStatus::kWordsInvalid};
  }

  int move_points = 0;
  for (const auto& word : words) {
    move_points += word.points();
  }

  // Place the move after checking errors
  for (const auto& [tile, row, col] : move) {
    PlaceTile(tile, row, col);
  }

  return {words, move_points, ResponseStatus::kSuccess};
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