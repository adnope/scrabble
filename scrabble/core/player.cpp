#include "player.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "bag.hpp"
#include "spdlog/spdlog.h"
#include "tile.hpp"

namespace core {
Player::Player(const std::string &player_name, const int score)
    : player_name_(player_name), player_score_(score) {}

void Player::AddTile(const Tile &tile) {
  for (Tile &t : deck_) {
    if (t.letter() == 0) {
      t = tile;
      return;
    }
  }
  spdlog::error("[Player.AddTile()] No available slots to add tile: {}",
                tile.letter());
}

void Player::AddTiles(const std::vector<Tile> &tiles) {
  for (const Tile &tile : tiles) {
    AddTile(tile);
  }
}

void Player::DrawNewTiles(Bag &bag) {
  int num_tiles =
      std::min(kMaxDeckSize - current_deck_size(), bag.num_tiles_remanining());

  for (int i = 0; i < num_tiles; ++i) {
    Tile drawn_tile = bag.DrawTile();
    for (Tile &tile : deck_) {
      if (tile.IsEmpty()) {
        tile = drawn_tile;
        break;
      }
    }
  }
}

void Player::UseTile(const int index) { deck_.at(index) = Tile(); }

void Player::SwapTile(const int index, Bag &bag) {
  bag.AddTile(deck_.at(index));
  const Tile drawn_tile = bag.DrawTile();
  deck_.at(index) = drawn_tile;
}

// Return true if swap successfully, false if failed
bool Player::PerformSwap(Bag &bag, const std::vector<int> &indices) {
  if (static_cast<int>(indices.size()) > bag.num_tiles_remanining()) {
    return false;
  }

  for (const int index : indices) {
    SwapTile(index, bag);
  }

  return true;
}

bool Player::PerformMove(const std::vector<Placement> &move, Board &board) {
  auto temp_board = board;
  for (const auto& placement : move) {
    
  }
}

void Player::PrintDeck() const {
  std::string player_tiles;
  for (const auto &tile : deck_) {
    if (tile.IsEmpty()) {
      player_tiles += "[] ";
      continue;
    }
    player_tiles += tile.letter();
    player_tiles += std::to_string(tile.points());
    player_tiles += ' ';
  }
  std::cout << "Player: " << player_name_ << ", tiles: " << player_tiles
            << '\n';
}

int Player::GetDeckScore() const {
  int total_score = 0;
  for (size_t i = 0; i < deck_.size(); i++) {
    total_score += deck_.at(i).points();
  }
  return total_score;
}

int Player::current_deck_size() const {
  int count = 0;
  for (const Tile &tile : deck_) {
    if (!tile.IsEmpty()) {
      ++count;
    }
  }
  return count;
}
}  // namespace core