#include "player.hpp"

#include <string>
#include <vector>

#include "bag.hpp"
#include "spdlog/spdlog.h"
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
void Player::UseTile(const int index) {
  player_tiles_.erase(player_tiles_.begin() + index);
}

// Hàm này sẽ đưa viên gạch trong tay người chơi vào vector các viên gạch đã sử
// dụng Nếu từ trên ô hợp lệ -> xóa khỏi tay người chơi Khi đó usedTiles sẽ chứa
// các viên gạch đã sử dụng sẽ sử dụng trong tính điểm và xóa khỏi tay người
// chơi
void Player::PutTileToUsedTiles(const int index,
                                std::vector<Tile> &used_tiles) {
  used_tiles.push_back(player_tiles_[index]);
  player_tiles_.erase(player_tiles_.begin() + index);
}

// Hàm này lấy viên gạch tại index khỏi tay người chơi và đặt lại vào bag
void Player::SwapTile(const int index, Bag &bag) {
  bag.AddTile(player_tiles_[index]);
  player_tiles_.erase(player_tiles_.begin() + index);
}

bool Player::PerformSwap(Bag &bag, const std::vector<int> &indices) {
  const int num_tiles_to_draw = static_cast<int>(indices.size());
  if (num_tiles_to_draw > bag.num_tiles_remanining()) {
    return false;
  }
  for (const int index : indices) {
    SwapTile(index, bag);
  }
  const std::vector<Tile> tiles_drawn = bag.DrawTiles(num_tiles_to_draw);
  this->PutTilesInHand(tiles_drawn);
  return true;
}
void Player::PrintTilesInHand() const {
  std::string player_tiles;
  for (const auto &tile : player_tiles_) {
    player_tiles += tile.letter();
    player_tiles += std::to_string(tile.points());
    player_tiles += ' ';
  }
  spdlog::info("Player: {0}, tiles: {1}", player_name_, player_tiles);
}

int Player::GetHandScore() const {
  int total_score = 0;
  for (size_t i = 0; i < player_tiles_.size(); i++) {
    total_score += player_tiles_[i].points();
  }
  return total_score;
}

bool Player::ExecutePlaceMove(Bag &bag, const Dictionary &dictionary,
                              Board &board, const bool horizontal, int row,
                              int col, const std::vector<int> &tile_indices) {
  std::vector<std::string> words;
  std::vector<Tile> used_tiles;
  int turn_score = 0;
  for (size_t i = 0; i < tile_indices.size(); ++i) {
    PutTileToUsedTiles(tile_indices[i], used_tiles);
    if (used_tiles[used_tiles.size() - 1].IsBlank() &&
        i + 1 < tile_indices.size()) {
      // used_tiles[used_tiles.size() - 1].UseAs(word[i + 1]);
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
    spdlog::error("At least one tile must be adjacent to other tiles on the board.");
    return false;
  }

  // Word not in dictionary
  for (size_t i = 0; i < words.size(); i++) {
    const bool is_valid = dictionary.Contains(words[i]);
    if (!is_valid) {
      spdlog::error("Invalid words: {}", words[i]);
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
  spdlog::info("Score of this turn: {}", turn_score);

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