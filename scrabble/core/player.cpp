#include "player.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "bag.hpp"
#include "tile.hpp"

namespace core {
Player::Player(const std::string &player_name, const int score,
               const int number_of_tiles)
    : player_score_(score),
      num_tiles_(number_of_tiles),
      player_name_(player_name) {}

// Hàm này sẽ trả về số điểm của người chơi
void Player::GetTiles(Bag &bag, const int number_of_tiles) {
  auto tiles = bag.DrawTiles(number_of_tiles);
  for (Tile tile : tiles) {
    hand_tiles_.push_back(tile);
  }
}

// Hàm này sẽ sử dụng viên gạch trong tay người chơi
void Player::UseTiles(const char c) {
  for (int i = 0; i < static_cast<int>(hand_tiles_.size()); ++i) {
    if (c == hand_tiles_[i].letter()) {
      hand_tiles_.erase(hand_tiles_.begin() + i);
      break;
    }
  }
}

// Hàm này sẽ trao đổi một viên gạch với bag
void Player::ExchangeTile(const char c, Bag &bag) {
  int pos = 0;
  if (FindTile(c, pos)) {
    // Funtion AddTiles in Bag class nó là AddTiles(const Tile& tile),
    // AddTiles(const std::vector<Tile>& tiles) nên mình sẽ dùng AddTiles(const
    // Tile& tile) để thêm từng viên vào bag hiện tại nó không nhận 1 phẩn từ có
    // thể là do nhầm 2 funtion với nhau
    bag.AddTiles(hand_tiles_[pos]);
    hand_tiles_.erase(hand_tiles_.begin() + pos);
  }
}

// Hàm này sẽ tìm vị trí của viên gạch trong tay người chơi
bool Player::FindTile(const char c, int &pos) {
  for (int i = 0; i < static_cast<int>(hand_tiles_.size()); ++i) {
    if (i == pos) {
      continue;
    }
    if (c == hand_tiles_[i].letter()) {
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
void Player::ReturnTile(const char c, std::vector<Tile> &used_tiles) {
  int pos = 0;
  if (FindTile(c, pos)) {
    used_tiles.push_back(hand_tiles_[pos]);
    hand_tiles_.erase(hand_tiles_.begin() + pos);
  }
}

int Player::GetHandScore() const {
  int hand_score = 0;
  for (const Tile &tile : hand_tiles_) {
    hand_score += tile.points();
  }
  return hand_score;
}

void Player::ExecuteExchangeMove(Bag &bag, const std::string &word) {
  // Implementation of place move logic
  const int number_of_tiles_to_draw = static_cast<int>(word.length());
  for (int i = 0; i < number_of_tiles_to_draw; ++i) {
    this->ExchangeTile(word[i], bag);
  }
  this->GetTiles(bag, number_of_tiles_to_draw);
}

bool Player::ExecutePlaceMove(Bag &bag, Dictionary &dict, Board &board,
                              const char dir, int row, int col,
                              const std::string &word) {
  // Implementation of place move logic
  std::vector<std::string> words;
  std::vector<Tile> used_tiles;
  int word_score = 0;
  for (size_t i = 0; i < word.length(); ++i) {
    ReturnTile(word[i], used_tiles);
    if (used_tiles[used_tiles.size() - 1].IsBlank() && i + 1 < word.length()) {
      used_tiles[used_tiles.size() - 1].UseAs(word[i + 1]);
      i++;
    }
  }

  bool is_valid = true;
  // Tính điểm nếu từ hợp lệ
  words = board.GetAllWords(row, col, dir, word_score, used_tiles);

  for (size_t i = 0; i < words.size(); i++) {
    for (size_t j = 0; j < words[i].length(); j++) {
      words[i][j] = static_cast<char>(tolower(words[i][j]));
    }
  }

  if (words.empty()) {
    // Nen hien thi notify cho nguoi dung GUI
    std::cout
        << "At least one tile must be adjacent to other tiles on the board."
        << '\n';
    return false;
  }

  for (size_t i = 0; i < words.size(); i++) {
    is_valid = dict.Contains(words[i]);
    if (!is_valid) {
      // Nen hien thi notify cho nguoi dung GUI
      std::cout << "Invalid word: " << words[i] << '\n';
      return false;
    }
  }

  const int number_of_used_tiles = static_cast<int>(used_tiles.size());
  // Nếu tất cả các viên gạch đã sử dụng đều là viên gạch trong tay người chơi
  // bonus points
  if (number_of_used_tiles == num_tiles_) {
    constexpr int BONUS_SCORE = 50;
    word_score += BONUS_SCORE;
  }

  this->AddScore(word_score);
  // Hien thi GUI
  std::cout << "score of this round: " << word_score << '\n';

  int i = 0;
  // Da dat len ban
  bool is_placed = false;
  while (i < number_of_used_tiles) {
    const char tile_letter = used_tiles[i].letter();
    is_placed = board.IsOccupiedAt(row - 1, col - 1);
    if (dir == '-') {
      if (!is_placed) {
        // Need to check use board vector or not
        // board.GetBoard().at(row - 1).at(col - 1).PlaceTile(usedTiles[i]);
        board.PlaceTile(used_tiles[i], row - 1, col - 1);
        this->UseTiles(tile_letter);
        i++;
      }
      col++;
    } else if (dir == '|') {
      if (!is_placed) {
        board.PlaceTile(used_tiles[i], row - 1, col - 1);
        this->UseTiles(tile_letter);
        i++;
      }
      row++;
    }

    const int number_of_tiles_remaining = bag.GetNumberOfTilesRemaining();
    if (number_of_used_tiles > number_of_tiles_remaining) {
      // Nen hien thi notify cho nguoi dung GUI
      this->GetTiles(bag, number_of_tiles_remaining);
    } else {
      this->GetTiles(bag, number_of_used_tiles);
    }
    board.set_first_move_status(false);
    return true;
  }
}
}  // namespace core
