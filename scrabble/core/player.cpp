#include "player.hpp"

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
    // need getLetter() function in Tile class
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
    // need getLetter() function in Tile class
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

void Player::SubtractScore(int score_to_subtract) { score -= score_to_subtract; }

int Player::GetHandScore() {
  int handScore = 0;
  for (const Tile &tile : hand_tiles) {
    // Need getPoints() function in Tile class
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
}  // namespace core
