#include "board.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "square.hpp"
#include "tile.hpp"

namespace core {
Board::Board() : is_first_move_(true) {
  std::string default_board =
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

  for (int i = 0; i < HEIGHT; ++i) {
    getline(board_stream, row);
    for (int j = 0; j < WIDTH; ++j) {
      if (i == START_POS_Y && j == START_POS_X) {
        Square s(i, j, "***", "2n");
        board_.at(i).at(j) = s;
      } else {
        switch (row[j]) {
          case '.': {
            Square square(i, j, "...", "1");
            board_.at(i).at(j) = square;
            break;
          }
          case '2': {
            Square square(i, j, "2L ", "2");
            board_.at(i).at(j) = square;
            break;
          }
          case '3': {
            Square square(i, j, "3L ", "3");
            board_.at(i).at(j) = square;
            break;
          }
          case 'd': {
            Square square(i, j, "2W ", "2n");
            board_.at(i).at(j) = square;
            break;
          }
          case 't': {
            Square square(i, j, "3W ", "3n");
            board_.at(i).at(j) = square;
            break;
          }
          default:
            std::cerr << "Invalid character in board declaration.\n";
        }
      }
    }
  }
}

// int Board::getRowSize() const { return WIDTH; }

// int Board::getColSize() const { return HEIGHT; }

// int Board::getStartX() const { return START_POS_X; }

// int Board::getStartY() const { return START_POS_Y; }

// This function finds vertical words connected to newly placed horizontal tiles
std::string Board::VerticalWord(Tile tile, const int row, const int col,
                                int& points) {
  std::string word;
  if (tile.IsBlank()) {
    word += tile.GetUse();
  } else {
    word += tile.GetLetter();
  }
  points += tile.GetPoints();
  int up = row - 1;
  int down = row + 1;
  while (up >= 0 && board_.at(up).at(col).IsOccupied()) {
    word.insert(0, board_.at(up).at(col).GetValue());
    points += board_.at(up).at(col).GetTilePoints();
    --up;
  }
  while (down <= WIDTH - 1 && board_.at(down).at(col).IsOccupied()) {
    word += board_.at(down).at(col).GetValue();
    points += board_.at(down).at(col).GetTilePoints();
    ++down;
  }
  return word;
}

// this function finds horizontal words connected to newly placed vertical tiles
std::string Board::HorizontalWord(Tile t, const int row, const int col,
                                  int& points) {
  std::string word;
  if (t.IsBlank()) {
    word += t.GetUse();
  } else {
    word += t.GetLetter();
  }
  points += t.GetPoints();
  int right = col + 1;
  int left = col - 1;
  while (left >= 0 && board_.at(row).at(left).IsOccupied()) {
    word.insert(0, board_.at(row).at(left).GetValue());
    points += board_.at(row).at(left).GetTilePoints();
    --left;
  }
  while (right <= HEIGHT - 1 && board_.at(row).at(right).IsOccupied()) {
    word += board_.at(row).at(right).GetValue();
    points += board_.at(row).at(right).GetTilePoints();
    ++right;
  }
  return word;
}

// this function finds the main word created by placed tiles
// makes calls to find other words formed by a move
// also scores every word that is created by a move
std::vector<std::string> Board::AllWords(const int row, const int col,
                                         const char dir, int& score,
                                         std::vector<Tile>& usedTiles) {
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
    tempWord += usedTiles[0].GetUse();
  } else {
    tempWord += usedTiles[0].GetLetter();
  }
  std::string mult = board_.at(row - 1).at(col - 1).GetMultiplier();
  if (mult == "3n") {
    multiplier *= 3;
    score += (3 * usedTiles[0].GetPoints());
  } else if (mult == "2n") {
    multiplier *= 2;
    score += (2 * usedTiles[0].GetPoints());

  } else if (mult == "3") {
    score += (3 * usedTiles[0].GetPoints());
  } else if (mult == "2") {
    score += (2 * usedTiles[0].GetPoints());
  } else {
    score += (usedTiles[0].GetPoints()) * multiplier;
  }

  if (dir == '-') {
    points = 0;
    // check if there is are words in vertical direction
    word2 = VerticalWord(usedTiles[0], row - 1, col - 1, points);
    if (word2.length() != 1) {
      points *= multiplier;
      score += points;
      words.push_back(word2);
    }
    while (left >= 0 && right < HEIGHT) {
      // add letters left adjacent to first placed tile
      if (board_.at(row - 1).at(left).IsOccupied()) {
        isValid = true;
        tempWord.insert(0, board_.at(row - 1).at(left).GetValue());
        score += (board_.at(row - 1).at(left).GetTilePoints()) * multiplier;
        if (left != 0) {
          left--;
          continue;
        }
      }
      // add letters right adjacent to placed tiles
      if (board_.at(row - 1).at(right).IsOccupied()) {
        isValid = true;
        tempWord += board_.at(row - 1).at(right).GetValue();
        score += (board_.at(row - 1).at(right).GetTilePoints()) * multiplier;
        if (right != HEIGHT - 1) {
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
          tempWord += usedTiles[counter].GetUse();
        } else {
          tempWord += usedTiles[counter].GetLetter();
        }
        mult = board_.at(row - 1).at(right).GetMultiplier();
        if (mult == "3n") {
          secondaryMultiplier *= 3;
          multiplier *= 3;
          score += (usedTiles[counter].GetPoints()) * multiplier;
        } else if (mult == "2n") {
          secondaryMultiplier *= 2;
          multiplier *= 2;
          score += (usedTiles[counter].GetPoints()) * multiplier;
        } else if (mult == "3") {
          score += (3 * usedTiles[counter].GetPoints()) * multiplier;
        } else if (mult == "2") {
          score += (2 * usedTiles[counter].GetPoints()) * multiplier;
        } else {
          score += (usedTiles[counter].GetPoints()) * multiplier;
        }
        points = 0;
        word2 = VerticalWord(usedTiles[counter], row - 1, right, points);
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
  else if (dir == '|') {
    points = 0;
    word2 = HorizontalWord(usedTiles[0], row - 1, col - 1, points);
    if (word2.length() != 1) {
      points *= multiplier;
      score += points;
      words.push_back(word2);
    }
    while (up >= 0 && down < WIDTH) {
      if (board_.at(up).at(col - 1).IsOccupied()) {
        isValid = true;
        tempWord.insert(0, board_.at(up).at(col - 1).GetValue());
        score += (board_.at(up).at(col - 1).GetTilePoints()) * multiplier;
        up--;
        continue;
      }
      if (board_.at(down).at(col - 1).IsOccupied()) {
        isValid = true;
        tempWord += board_.at(down).at(col - 1).GetValue();
        score += (board_.at(down).at(col - 1).GetTilePoints()) * multiplier;
        down++;
        continue;
      }
      if (counter >= wordLen) {
        break;
      }
      if (usedTiles[counter].IsBlank()) {
        tempWord += usedTiles[counter].GetUse();
      } else {
        tempWord += usedTiles[counter].GetLetter();
      }
      mult = board_.at(down).at(col - 1).GetMultiplier();
      if (mult == "3n") {
        secondaryMultiplier *= 3;
        multiplier *= 3;
        score += (usedTiles[counter].GetPoints()) * multiplier;
      } else if (mult == "2n") {
        secondaryMultiplier *= 2;
        multiplier *= 2;
        score += (usedTiles[counter].GetPoints()) * multiplier;
      } else if (mult == "3") {
        score += (3 * usedTiles[counter].GetPoints()) * multiplier;
      } else if (mult == "2") {
        score += (2 * usedTiles[counter].GetPoints()) * multiplier;
      } else {
        score += (usedTiles[counter].GetPoints()) * multiplier;
      }
      points = 0;
      word2 = HorizontalWord(usedTiles[counter], down, col - 1, points);
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