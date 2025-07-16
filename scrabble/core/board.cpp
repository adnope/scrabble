#include "board.hpp"

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

  std::istringstream board_stream(default_board);
  std::string row;
  for (int i = 0; i < kHeight; ++i) {
    getline(board_stream, row);
    for (int j = 0; j < kWidth; ++j) {
      if (i == kStartPosColumn && j == kStartPosRow) {
        Square square(i, j, "***", "2n");
        board_grid_.at(i).at(j) = square;
      } else {
        switch (row[j]) {
          case '.': {
            Square square(i, j, "...", "1");
            board_grid_.at(i).at(j) = square;
            break;
          }
          case '2': {
            Square square(i, j, "2L ", "2");
            board_grid_.at(i).at(j) = square;
            break;
          }
          case '3': {
            Square square(i, j, "3L ", "3");
            board_grid_.at(i).at(j) = square;
            break;
          }
          case 'd': {
            Square square(i, j, "2W ", "2n");
            board_grid_.at(i).at(j) = square;
            break;
          }
          case 't': {
            Square square(i, j, "3W ", "3n");
            board_grid_.at(i).at(j) = square;
            break;
          }
          default:
            std::cerr << "Invalid character in board declaration.\n";
        }
      }
    }
  }
}

// This function finds vertical words connected to newly placed horizontal tiles
std::string Board::GetVerticalWord(const Tile tile, const int row,
                                   const int col, int& points) {
  std::string word;
  if (tile.IsBlank()) {
    word += tile.get_use();
  } else {
    word += tile.letter();
  }
  points += tile.points();
  int up = row - 1;
  int down = row + 1;
  while (up >= 0 && board_grid_.at(up).at(col).IsOccupied()) {
    word.insert(0, board_grid_.at(up).at(col).value());
    points += board_grid_.at(up).at(col).tile_points();
    --up;
  }
  while (down <= kWidth - 1 && board_grid_.at(down).at(col).IsOccupied()) {
    word += board_grid_.at(down).at(col).value();
    points += board_grid_.at(down).at(col).tile_points();
    ++down;
  }
  return word;
}

// this function finds horizontal words connected to newly placed vertical tiles
std::string Board::GetHorizontalWord(const Tile tile, const int row,
                                     const int col, int& points) {
  std::string word;
  if (tile.IsBlank()) {
    word += tile.get_use();
  } else {
    word += tile.letter();
  }
  points += tile.points();
  int right = col + 1;
  int left = col - 1;
  while (left >= 0 && board_grid_.at(row).at(left).IsOccupied()) {
    word.insert(0, board_grid_.at(row).at(left).value());
    points += board_grid_.at(row).at(left).tile_points();
    --left;
  }
  while (right <= kHeight - 1 && board_grid_.at(row).at(right).IsOccupied()) {
    word += board_grid_.at(row).at(right).value();
    points += board_grid_.at(row).at(right).tile_points();
    ++right;
  }
  return word;
}

// this function finds the main word created by placed tiles
// makes calls to find other words formed by a move
// also scores every word that is created by a move
std::vector<std::string> Board::GetAllWords(
    const int row, const int col, const bool horizontal, int& score,
    const std::vector<Tile>& usedTiles) {
  std::vector<std::string> words;
  std::string tempWord;
  int multiplier = 1;
  int secondaryMultiplier = 1;
  int points = 0;
  std::string word2;
  int right = col;
  int left = col - 2;
  if (left == -1) {
    left = 0;
  }
  int up = row - 2;
  if (up == -1) {
    up = 0;
  }
  int down = row;
  int counter = 1;
  bool isValid = false;
  int wordLen = static_cast<int>(usedTiles.size());
  if (usedTiles[0].IsBlank()) {
    tempWord += usedTiles[0].get_use();
  } else {
    tempWord += usedTiles[0].letter();
  }
  std::string mult = board_grid_.at(row - 1).at(col - 1).multiplier();
  if (mult == "3n") {
    multiplier *= 3;
    score += (3 * usedTiles[0].points());
  } else if (mult == "2n") {
    multiplier *= 2;
    score += (2 * usedTiles[0].points());

  } else if (mult == "3") {
    score += (3 * usedTiles[0].points());
  } else if (mult == "2") {
    score += (2 * usedTiles[0].points());
  } else {
    score += (usedTiles[0].points()) * multiplier;
  }

  if (horizontal) {
    points = 0;
    // check if there is are words in vertical direction
    word2 = GetVerticalWord(usedTiles[0], row - 1, col - 1, points);
    if (word2.length() != 1) {
      points *= multiplier;
      score += points;
      words.push_back(word2);
    }
    while (left >= 0 && right < kHeight) {
      // add letters left adjacent to first placed tile
      if (board_grid_.at(row - 1).at(left).IsOccupied()) {
        isValid = true;
        tempWord.insert(0, board_grid_.at(row - 1).at(left).value());
        score += (board_grid_.at(row - 1).at(left).tile_points()) * multiplier;
        if (left != 0) {
          left--;
          continue;
        }
      }
      // add letters right adjacent to placed tiles
      if (board_grid_.at(row - 1).at(right).IsOccupied()) {
        isValid = true;
        tempWord += board_grid_.at(row - 1).at(right).value();
        score += (board_grid_.at(row - 1).at(right).tile_points()) * multiplier;
        if (right != kHeight - 1) {
          right++;
          continue;
        }
      }
      // else use placed tiles to form word
      else {
        if (counter >= wordLen) {
          break;
        }
        if (usedTiles[counter].IsBlank()) {
          tempWord += usedTiles[counter].get_use();
        } else {
          tempWord += usedTiles[counter].letter();
        }
        mult = board_grid_.at(row - 1).at(right).multiplier();
        if (mult == "3n") {
          secondaryMultiplier *= 3;
          multiplier *= 3;
          score += (usedTiles[counter].points()) * multiplier;
        } else if (mult == "2n") {
          secondaryMultiplier *= 2;
          multiplier *= 2;
          score += (usedTiles[counter].points()) * multiplier;
        } else if (mult == "3") {
          score += (3 * usedTiles[counter].points()) * multiplier;
        } else if (mult == "2") {
          score += (2 * usedTiles[counter].points()) * multiplier;
        } else {
          score += (usedTiles[counter].points()) * multiplier;
        }
        points = 0;
        word2 = GetVerticalWord(usedTiles[counter], row - 1, right, points);
        if (word2.length() != 1) {
          points *= secondaryMultiplier;
          score += points;
          words.push_back(word2);
        }
        right++;
        counter++;
      }
    }
    // if not first move and no adjacent tiles, return empty vector
    if (!isValid && !is_first_move_) {
      return words;
    }
    // add word to be checked by dictionary
    words.push_back(tempWord);
  }
  // same as above, replace left with up & right with down
  // forms main word in vertical direction and connected horizontal words
  else if (!horizontal) {
    points = 0;
    word2 = GetHorizontalWord(usedTiles[0], row - 1, col - 1, points);
    if (word2.length() != 1) {
      points *= multiplier;
      score += points;
      words.push_back(word2);
    }
    while (up >= 0 && down < kWidth) {
      if (board_grid_.at(up).at(col - 1).IsOccupied()) {
        isValid = true;
        tempWord.insert(0, board_grid_.at(up).at(col - 1).value());
        score += (board_grid_.at(up).at(col - 1).tile_points()) * multiplier;
        up--;
        continue;
      }
      if (board_grid_.at(down).at(col - 1).IsOccupied()) {
        isValid = true;
        tempWord += board_grid_.at(down).at(col - 1).value();
        score += (board_grid_.at(down).at(col - 1).tile_points()) * multiplier;
        down++;
        continue;
      }
      if (counter >= wordLen) {
        break;
      }
      if (usedTiles[counter].IsBlank()) {
        tempWord += usedTiles[counter].get_use();
      } else {
        tempWord += usedTiles[counter].letter();
      }
      mult = board_grid_.at(down).at(col - 1).multiplier();
      if (mult == "3n") {
        secondaryMultiplier *= 3;
        multiplier *= 3;
        score += (usedTiles[counter].points()) * multiplier;
      } else if (mult == "2n") {
        secondaryMultiplier *= 2;
        multiplier *= 2;
        score += (usedTiles[counter].points()) * multiplier;
      } else if (mult == "3") {
        score += (3 * usedTiles[counter].points()) * multiplier;
      } else if (mult == "2") {
        score += (2 * usedTiles[counter].points()) * multiplier;
      } else {
        score += (usedTiles[counter].points()) * multiplier;
      }
      points = 0;
      word2 = GetHorizontalWord(usedTiles[counter], down, col - 1, points);
      if (word2.length() != 1) {
        points *= secondaryMultiplier;
        score += points;
        words.push_back(word2);
      }
      down++;
      counter++;
    }
    if (!isValid && !is_first_move_) {
      return words;
    }
    words.push_back(tempWord);
  }
  return words;
}
}  // namespace core