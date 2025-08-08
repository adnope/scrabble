#include "board.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "square.hpp"
#include "tile.hpp"

namespace core {
Board::Board() {
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
  if (row < 0 || row >= kHeight || col < 0 || col >= kWidth) {
    return false;
  }

  if (!IsOccupied(row, col)) {
    if (is_first_move_) {
      is_first_move_ = false;
    }
    board_grid_.at(row).at(col).PlaceTile(tile);
    return true;
  }
  std::cout << "Failed to place tile at: " << row << " " << col << '\n';
  return false;
}

Word Board::GetWordFromPos(int row, int col, const bool horizontal,
                           const Move& move) {
  // Go backward to find start
  int start_row = row;
  int start_col = col;
  while (start_row >= 0 && start_row < kHeight && start_col >= 0 &&
         start_col < kWidth &&
         board_grid_.at(start_row).at(start_col).IsOccupied()) {
    if (horizontal) {
      --start_col;
    } else {
      --start_row;
    }
  }

  if (horizontal) {
    start_col++;

    start_col = std::max(start_col, 0);
  } else {
    start_row++;
    start_row = std::max(start_row, 0);
  }

  if (start_row < 0 || start_row >= kHeight || start_col < 0 ||
      start_col >= kWidth) {
    return Word();
  }
  Word word;
  int letter_index = 0;
  int r = start_row;
  int c = start_col;
  while (r < kHeight && c < kWidth && row >= 0 && col >= 0 &&
         board_grid_.at(r).at(c).IsOccupied()) {
    const Square square = board_grid_.at(r).at(c);
    const char letter = square.tile_letter();
    const auto multiplier = square.multiplier();
    int points = square.tile_points();
    word.AddToContent(letter, Square::Multiplier::kNormal);
    bool is_newly_placed_tile = false;
    for (const auto& placement : move) {
      if (r == placement.row && c == placement.col) {
        is_newly_placed_tile = true;
        break;
      }
    }
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
      ++c;
    } else {
      ++r;
    }
    ++letter_index;
  }
  word.ApplyMultiplier();
  return word;
}

std::vector<Word> Board::GetWordsFromMove(const Move& move,
                                          const bool horizontal) {
  auto backup_board = board_grid_;
  bool is_first_move_backup = is_first_move_;

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
  is_first_move_ = is_first_move_backup;
  return words;
}

bool Board::IsMoveOccupied(const Move& move) const {
  return std::any_of(move.begin(), move.end(), [this](const auto& placement) {
    return IsOccupied(placement.row, placement.col);
  });
}

bool Board::IsInStartingSquare(const Move& move) {
  return std::any_of(move.begin(), move.end(), [](const Placement& p) {
    return p.row == kStartPosRow && p.col == kStartPosColumn;
  });
}

bool Board::AdjacentToAnotherInMove(const int row, const int col,
                                    const Move& move) {
  return std::any_of(move.begin(), move.end(),
                     [row, col](const auto& placement) {
                       auto& [t, m_row, m_col] = placement;
                       return (m_row - 1 == row && m_col == col) ||
                              (m_row + 1 == row && m_col == col) ||
                              (m_row == row && m_col + 1 == col) ||
                              (m_row == row && m_col - 1 == col);
                     });
}

bool Board::AdjacentToBoard(const int row, const int col) const {
  for (int i = 0; i < 15; ++i) {
    for (int j = 0; j < 15; ++j) {
      if (!board_grid_.at(i).at(j).IsOccupied()) {
        continue;
      }
      if (i - 1 == row && j == col) {
        return true;
      }
      if (i + 1 == row && j == col) {
        return true;
      }
      if (i == row && j + 1 == col) {
        return true;
      }
      if (i == row && j - 1 == col) {
        return true;
      }
    }
  }
  return false;
}

bool Board::IsAdjacentToExistingTiles(const Move& move) const {
  if (is_first_move_) {
    return std::all_of(move.begin(), move.end(), [move](const Placement& p) {
      return AdjacentToAnotherInMove(p.row, p.col, move);
    });
  }
  int adjacent_to_board_count = 0;
  for (const auto& [t, row, col] : move) {
    if (AdjacentToBoard(row, col)) {
      ++adjacent_to_board_count;
    }
  }

  return std::all_of(move.begin(), move.end(),
                     [move, this](const Placement& p) {
                       return AdjacentToAnotherInMove(p.row, p.col, move) ||
                              AdjacentToBoard(p.row, p.col);
                     }) &&
         adjacent_to_board_count != 0;
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
        // std::cout << "Move placements not aligned\n";
        return -1;
      }
    }
  }

  return static_cast<int>(horizontal);
}

bool Board::AreInDictionary(const std::vector<std::string>& words,
                            const Lexicon& lexicon, std::string& invalid_word) {
  if (words.empty()) {
    invalid_word = "1 letter word is not allowed";
    return false;
  }
  for (auto word : words) {
    std::transform(word.begin(), word.end(), word.begin(), tolower);
    if (!lexicon.Contains(word)) {
      invalid_word = word;
      return false;
    }
  }
  return true;
}

Board::MoveValidationResponse Board::ValidateMove(const Move& move,
                                                  const Lexicon& lexicon) {
  // Check if first move is on the starting square
  if (is_first_move_ && !IsInStartingSquare(move)) {
    return {{}, 0, ResponseStatus::kNotInStartingSquare, ""};
  }

  // Check if any placement is performed on occupied square
  if (IsMoveOccupied(move)) {
    return {{}, 0, ResponseStatus::kOccupied, ""};
  }

  // Check if the move has at least 1 tile adjacent to existing tiles
  if (!IsAdjacentToExistingTiles(move)) {
    return {{}, 0, ResponseStatus::kNotAdjacent, ""};
  }

  // Checking placements alignment, return the type of alignment
  int horizontal = IsAligned(move);
  if (horizontal == -1) {
    return {{}, 0, ResponseStatus::kNotAligned, ""};
  }

  // Checking words validity
  const auto words = GetWordsFromMove(move, static_cast<bool>(horizontal));
  std::vector<std::string> word_list;
  word_list.reserve(words.size());
  for (const auto& word : words) {
    word_list.push_back(word.AsString());
  }
  std::string invalid_word;
  if (!AreInDictionary(word_list, lexicon, invalid_word)) {
    return {words, 0, ResponseStatus::kWordsInvalid, invalid_word};
  }

  int move_points = 0;
  for (const auto& word : words) {
    move_points += word.points();
  }

  // Place the move after checking errors
  for (const auto& [tile, row, col] : move) {
    PlaceTile(tile, row, col);
  }

  return {words, move_points, ResponseStatus::kSuccess, ""};
}

std::array<std::array<std::string, 15>, 15> Board::GetDisplayFormat() const {
  std::array<std::array<std::string, 15>, 15> result;
  for (int row = 0; row < 15; ++row) {
    for (int col = 0; col < 15; ++col) {
      auto square = board_grid_.at(row).at(col);
      if (square.IsOccupied()) {
        result.at(row).at(col) = square.tile_letter();
      } else {
        Square::Symbol symbol = square.symbol();
        switch (symbol) {
          case Square::Symbol::kNormal:
            result.at(row).at(col) = ".";
            break;
          case Square::Symbol::kStart:
            result.at(row).at(col) = "*";
            break;
          case Square::Symbol::kDoubleLetter:
            result.at(row).at(col) = "DL";
            break;
          case Square::Symbol::kDoubleWord:
            result.at(row).at(col) = "DW";
            break;
          case Square::Symbol::kTripleLetter:
            result.at(row).at(col) = "TL";
            break;
          case Square::Symbol::kTripleWord:
            result.at(row).at(col) = "TW";
            break;
        }
      }
    }
  }

  return result;
}

Tile Board::GetTile(int row, int col) const {
  if (row < 0 || row >= kHeight || col < 0 || col >= kWidth) {
    return Tile();
  }
  return board_grid_.at(row).at(col).tile();
}

Tile Board::GetTile(int case_curr) const {
  int row = case_curr / kWidth;
  int col = case_curr % kWidth;
  if (row < 0 || row >= kHeight || col < 0 || col >= kWidth) {
    throw std::out_of_range("Invalid case_curr");
  }
  return board_grid_.at(row).at(col).tile();
}

}  // namespace core
