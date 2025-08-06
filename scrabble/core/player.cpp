#include "player.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "bag.hpp"
#include "core/board.hpp"
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

Player::MoveSubmissionResponse Player::SubmitMove(const Move &move,
                                                  Board &board,
                                                  const Lexicon &lexicon) {
  // Convert Player::Move to Board::Move
  std::vector<Board::Placement> board_move;
  board_move.reserve(move.size());
  for (const auto &[index, use, row, col] : move) {
    if (!deck_.at(index).IsBlankTile()) {
      board_move.emplace_back(deck_.at(index), row, col);
    } else {
      Tile tile(use, 0);
      board_move.emplace_back(tile, row, col);
    }
  }

  // Get board validation response
  auto board_response = board.ValidateMove(board_move, lexicon);
  MoveSubmissionResponse response(board_response);

  if (response.status != Board::ResponseStatus::kSuccess) {
    return response;
  }

  // Actually play the move if it is valid
  AddScore(response.move_points);
  for (const auto &placement : move) {
    UseTile(placement.tile_index);
  }
  return response;
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

Board::Move Player::ConvertToBoardMove(Player& player ,const Player::Move &player_move) {
    Board::Move board_move;
    const auto& handtiles = player.deck();
    for (const auto& placement : player_move) {
        if (placement.tile_index >= 0 &&
            static_cast<size_t>(placement.tile_index) < handtiles.size() &&
            !handtiles.at(placement.tile_index).IsEmpty()) {
            board_move.push_back(
                {handtiles.at(placement.tile_index), placement.row, placement.col});
        }
    }
    return board_move;
}
}  // namespace core
