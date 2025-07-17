#include "game.hpp"

#include "bag.hpp"
#include "board.hpp"
#include "dictionary.hpp"
#include "player.hpp"
#include "spdlog/spdlog.h"

namespace core {
Game::Game(int num_players, Dictionary::DictionaryType dict_type,
           const std::vector<std::string>& custom_names)
    : current_player_index_(0), consecutive_passes_(0), game_over_(false) {
  bag_ = Bag();
  board_ = Board();
  dictionary_ = Dictionary(dict_type);

  // Khoi tao nguoi choi
  // Cần suy nghĩ thêm.
}

bool Game::IsGameOver() const {
  if (bag_.num_tiles_remanining() == 0) {
    for (const auto& player : players_) {
      if (player.num_tiles_in_hand() == 0) {
        return true;
      }
    }
    if (consecutive_passes_ >= 2 * players_.size()) {
      return true;
    }
  }
  return game_over_;
}

void Game::NextTurn() {
  current_player_index_ = (current_player_index_ + 1) % players_.size();
  consecutive_passes_ = 0;
  spdlog::info("[Game] Turn switched to player: {}",
               players_[current_player_index_].name());
}

Player Game::GetWinner() const {
  Player winner = players_[0];
  for (const auto& player : players_) {
    if (player.score() > winner.score()) {
      winner = player;
    }
  }
  return winner;
}

const Player& Game::GetCurrentPlayer() const {
  return players_[current_player_index_];
}

int Game::GetBagSize() const { return bag_.num_tiles_remanining(); }

void Game::EndGame() {
  for (auto& player : players_) {
    int hand_score = 0;
    for (const Tile& tile : player.player_tiles()) {
      hand_score += tile.points();
    }

    player.AddScore(-hand_score);
    spdlog::info("[Game] Player {} final score adjusted by -{}: {}",
                 player.name(), hand_score, player.score());
  }
  game_over_ = true;
  spdlog::info("[Game] Game ended. Winner: {}", GetWinner().name());
}

}  // namespace core