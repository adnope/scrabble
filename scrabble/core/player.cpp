#include "player.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "bag.hpp"
#include "tile.hpp"

namespace core {
Player::Player(const std::string &player_name, const int score,
               const int number_of_tiles)
    : player_name_(player_name),
      player_score_(score),
      num_tiles_(number_of_tiles) {}

// Put tiles into player's hand
void Player::PutTilesInHand(const std::vector<Tile> &tiles) {
  for (const auto &tile : tiles) {
    player_tiles_.push_back(tile);
  }
}

// Use a tile in the player's hand
void Player::UseTile(const char c) {
  for (int i = 0; i < static_cast<int>(player_tiles_.size()); ++i) {
    if (c == player_tiles_[i].letter()) {
      player_tiles_.erase(player_tiles_.begin() + i);
      break;
    }
  }
}

// Hàm này sẽ trao đổi một viên gạch với bag
void Player::ExchangeTile(const char c, Bag &bag) {
  int pos = 0;
  if (FindTile(c, pos)) {
    bag.AddTiles(player_tiles_[pos]);
    player_tiles_.erase(player_tiles_.begin() + pos);
  }
}

// Hàm này sẽ tìm vị trí của viên gạch trong tay người chơi
bool Player::FindTile(const char c, int &pos) const {
  for (int i = 0; i < static_cast<int>(player_tiles_.size()); ++i) {
    if (i == pos) {
      continue;
    }
    if (c == player_tiles_[i].letter()) {
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
    used_tiles.push_back(player_tiles_[pos]);
    player_tiles_.erase(player_tiles_.begin() + pos);
  }
}

void Player::ExecuteExchangeMove(Bag &bag, const std::string &word) {
  // Implementation of place move logic
  const int number_of_tiles_to_draw = static_cast<int>(word.length());
  for (int i = 0; i < number_of_tiles_to_draw; ++i) {
    this->ExchangeTile(word[i], bag);
  }
  const std::vector<Tile> tiles = bag.DrawTiles(number_of_tiles_to_draw);
  this->PutTilesInHand(tiles);
}

int Player::GetHandScore() const {
  int tilesScore = 0;
  for(size_t i = 0; i < player_tiles_.size(); i++){
    tilesScore += player_tiles_[i].points();
  }
  return tilesScore;
}

bool Player::ExecutePlaceMove(Bag &bag, const Dictionary &dictionary,
                              Board &board, const bool horizontal, int row,
                              int col, const std::string &word) {
  std::vector<std::string> words;
  std::vector<Tile> used_tiles;
  int turn_score = 0;
  for (size_t i = 0; i < word.length(); ++i) {
    ReturnTile(word[i], used_tiles);
    if (used_tiles[used_tiles.size() - 1].IsBlank() && i + 1 < word.length()) {
      used_tiles[used_tiles.size() - 1].UseAs(word[i + 1]);
      i++;
    }
  }

  // Get all the words constructed from move
  words = board.GetAllWords(row, col, horizontal, turn_score, used_tiles);

  for (size_t i = 0; i < words.size(); i++) {
    for (size_t j = 0; j < words[i].length(); j++) {
      words[i][j] = static_cast<char>(tolower(words[i][j]));
    }
  }

  // Found no words from move
  if (words.empty()) {
    std::cout
        << "At least one tile must be adjacent to other tiles on the board."
        << '\n';
    return false;
  }

  // Word not in dictionary
  for (size_t i = 0; i < words.size(); i++) {
    const bool is_valid = dictionary.Contains(words[i]);
    if (!is_valid) {
      std::cout << "Invalid word: " << words[i] << '\n';
      return false;
    }
  }

  // Add special 50 points bonus
  const int number_of_used_tiles = static_cast<int>(used_tiles.size());
  if (number_of_used_tiles == num_tiles_) {
    constexpr int BONUS_SCORE = 50;
    turn_score += BONUS_SCORE;
  }

  this->AddScore(turn_score);
  std::cout << "score of this turn: " << turn_score << '\n';

  int i = 0;
  // Check if board is occupied at the positions
  bool is_occupied = false;
  while (i < number_of_used_tiles) {
    const char tile_letter = used_tiles[i].letter();
    is_occupied = board.IsOccupiedAt(row - 1, col - 1);
    if (horizontal) {
      if (!is_occupied) {
        // Need to check use board vector or not
        board.PlaceTile(used_tiles[i], row - 1, col - 1);
        this->UseTile(tile_letter);
        i++;
      }
      col++;
    } else if (!horizontal) {
      if (!is_occupied) {
        board.PlaceTile(used_tiles[i], row - 1, col - 1);
        this->UseTile(tile_letter);
        i++;
      }
      row++;
    }

    const int num_tiles_remaining = bag.num_tiles_remanining();
    if (number_of_used_tiles > num_tiles_remaining) {
      this->PutTilesInHand(bag.DrawTiles(bag.num_tiles_remanining()));
    } else {
      this->PutTilesInHand(bag.DrawTiles(number_of_used_tiles));
    }
    board.SetFirstMoveStatus(false);
    return true;
  }
}
}  // namespace core
