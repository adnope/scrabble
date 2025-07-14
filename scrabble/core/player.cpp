#include "player.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "bag.hpp"
#include "tile.hpp"


namespace core {
Player::Player(const std::string &player_name, int score, int number_of_tiles)
    : score(score), num_tiles(number_of_tiles), player_name(player_name) {}

// Hàm này sẽ trả về số điểm của người chơi
void Player::GetTiles(const std::vector<Tile> &tiles) {
  for (Tile tile : tiles) {
    hand_tiles.push_back(tile);
  }
}

// Hàm này sẽ sử dụng viên gạch trong tay người chơi
void Player::UseTiles(char c) {
  for (int i = 0; i < static_cast<int>(hand_tiles.size()); ++i) {
    if (c == hand_tiles[i].GetLetter()) {
      hand_tiles.erase(hand_tiles.begin() + i);
      break;
    }
  }
}

// Hàm này sẽ trao đổi một viên gạch với bag
void Player::ExchangeTile(char c, Bag &bag) {
  int pos = 0;
  if (FindTile(c, pos)) {
    // Funtion AddTiles in Bag class nó là AddTiles(const Tile& tile),
    // AddTiles(const std::vector<Tile>& tiles) nên mình sẽ dùng AddTiles(const
    // Tile& tile) để thêm từng viên vào bag hiện tại nó không nhận 1 phẩn từ có
    // thể là do nhầm 2 funtion với nhau
    bag.AddTiles(hand_tiles[pos]);
    hand_tiles.erase(hand_tiles.begin() + pos);
  }
}

// Hàm này sẽ tìm vị trí của viên gạch trong tay người chơi
bool Player::FindTile(char c, int &pos) {
  for (int i = 0; i < static_cast<int>(hand_tiles.size()); ++i) {
    if (i == pos) {
      continue;
    }
    if (c == hand_tiles[i].GetLetter()) {
      pos = i;
      return true;
    }
  }
  return false;
}

// Hàm này sẽ đưa viên gạch trong tay người chơi vào vector các viên gạch đã sử
// dụng Nếu từ trên ô hợp lệ -> xóa khỏi tay người chơi Khi đó usedTiles sẽ chứa
// các viên gạch đã sử dụng sẽ sử dụng trong tính điểm và xóa khỏi tay người
// chơi
void Player::ReturnTile(char c, std::vector<Tile> &used_tiles) {
  int pos = 0;
  if (FindTile(c, pos)) {
    used_tiles.push_back(hand_tiles[pos]);
    hand_tiles.erase(hand_tiles.begin() + pos);
  }
}

void Player::AddScore(int score_to_add) { score += score_to_add; }

void Player::SubtractScore(int score_to_subtract) {
  score -= score_to_subtract;
}

int Player::GetHandScore() {
  int handScore = 0;
  for (const Tile &tile : hand_tiles) {
    handScore += tile.GetPoints();
  }
  return handScore;
}

void Player::ExecuteExchangeMove(Bag &bag, std::string word) {
  // Implementation of place move logic
  for (int i = 0; i < static_cast<int>(word.length()); ++i) {
    this->ExchangeTile(word[i], bag);
  }
  this->GetTiles(bag.DrawTiles(static_cast<int>(word.length())));
}

bool Player::ExecutePlaceMove(Bag &bag, Dictionary &dict, Board &board,
                              char dir, int row, int col, std::string word) {
  // Implementation of place move logic
  std::vector<std::string> words;
  std::vector<Tile> usedTiles;
  int score = 0;
  for (int i = 0; i < static_cast<int>(word.length()); ++i) {
    ReturnTile(word[i], usedTiles);
    if (usedTiles[usedTiles.size() - 1].IsBlank() && i + 1 < word.length()) {
      usedTiles[usedTiles.size() - 1].UseAs(word[i + 1]);
      i++;
    }
  }

  bool isValid = true;
  // Tính điểm nếu từ hợp lệ
  words = board.AllWords(row, col, dir, score, usedTiles);

  for (int i = 0; i < words.size(); i++) {
    for (int j = 0; j < words[i].length(); j++) {
      words[i][j] = tolower(words[i][j]);
    }
  }

  if (words.empty()) {
    // Nen hien thi notify cho nguoi dung GUI
    std::cout
        << "At least one tile must be adjacent to other tiles on the board."
        << std::endl;
    return false;
  }

  for (int i = 0; i < words.size(); i++) {
    isValid = dict.Contains(words[i]);
    if (!isValid) {
      // Nen hien thi notify cho nguoi dung GUI
      std::cout << "Invalid word: " << words[i] << std::endl;
      return false;
    }
  }

  // Nếu tất cả các viên gạch đã sử dụng đều là viên gạch trong tay người chơi
  // bonus points
  if (usedTiles.size() == num_tiles) {
    score += 50;
  }

  this->AddScore(score);
  // Hien thi GUI
  std::cout << "score of this round: " << score << std::endl;

  int i = 0;
  // Da dat len ban
  bool isPlaced = false;
  while (i < usedTiles.size()) {
    isPlaced = board.boardVect[row - 1][col - 1]->IsOccupied();
    if (dir == '-') {
      if (!isPlaced) {
        // Need to check use board vector or not
        board.boardVect[row - 1][col - 1]->setValue(usedTiles[i]);
        this->UseTiles(usedTiles[i].GetLetter());
        i++;
      }
      col++;
    } else
      (dir == '|') {
        if (!isPlaced) {
          if (!isPlaced) {
            board.boardVect[row - 1][col - 1]->setValue(usedTiles[i]);
            this->UseTiles(usedTiles[i].GetLetter());
            i++;
          }
          row++;
        }
      }

    if (usedTiles.size() > bag.GetNumberOfTilesRemaining()) {
      // Nen hien thi notify cho nguoi dung GUI
      this->GetTiles(bag.DrawTiles(bag.GetNumberOfTilesRemaining()));
    } else {
      this->GetTiles(bag.DrawTiles(usedTiles.size()));
    }
    board.SetFirstMove(false);
    return true;
  }
}

}  // namespace core
