#include "game.hpp"
#include <algorithm>
#include <string>
#include "bag.hpp"
#include "board.hpp"
#include "dictionary.hpp"
#include "player.hpp"
#include "spdlog/spdlog.h"


namespace core {
Game::Game(int num_players, Dictionary::DictionaryType dict_type,
           const std::vector<std::string>& custom_names)
    : current_player_index_(0), consecutive_passes_(0), game_over_(false) {
  // Khoi tao moi truong
  bag_ = Bag();
  board_ = Board();
  dictionary_ = Dictionary(dict_type);
  move_history_ = std::vector<Move>();

  // Khoi tao nguoi choi
  for (int i = 0; i < num_players; i++) {
    std::string name = (i < custom_names.size() && !custom_names[i].empty())
                           ? custom_names[i]
                           : "Player" + std::to_string(i + 1);
    players_.push_back(Player(name, 0, 7));
    players_.back().PutTilesInHand(bag_.DrawTiles(7));
  }
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

void Game::ExecutePassMove() {
  Move move;
  move.player_name = players_[current_player_index_].name();
  move.is_pass = true;
  move.move_type = "pass";
  move.score = 0;
  move.words = {};
  move.tile_indices = {};
  move.row = 0;
  move.col = 0;
  move.horizontal = false;

  move_history_.push_back(move);
  consecutive_passes_++;
  spdlog::info("[Game] Player {} passed their turn", move.player_name);
  NextTurn();
}

bool Game::ExecuteSwapMove(const std::string& word, Move& move) {
  std::vector<int> tile_indices;
  if (!GetTileIndices(word, tile_indices)) {
    return false;
  }

  bool success =
      players_[current_player_index_].PerformSwap(bag_, tile_indices);
  if (success) {
    move.is_pass = false;
    move.score = 0;
    move.words = {word};
    move.tile_indices = tile_indices;
    move.row = 0;
    move.col = 0;
    move.horizontal = false;
    move_history_.push_back(move);
    NextTurn();
    spdlog::info("[Game] Player {} swapped tiles: {}", move.player_name, word);
  }
  return success;
}

bool Game::GetTileIndices(const std::string& word,
                          std::vector<int>& tile_indices) {
  const std::vector<Tile>& player_tiles =
      players_[current_player_index_].player_tiles();
  std::string word_upper = word;
  std::transform(word_upper.begin(), word_upper.end(), word_upper.begin(),
                 ::toupper);

  for (char c : word_upper) {
    bool found = false;
    for (size_t i = 0; i < player_tiles.size(); i++) {
      if ((player_tiles[i].letter() == c || player_tiles[i].IsBlank()) &&
          std::find(tile_indices.begin(), tile_indices.end(), i) ==
              tile_indices.end()) {
        tile_indices.push_back(i);
        found = true;
        break;
      }
    }
    if (!found) {
      spdlog::error("[Game] Player {} does not have required tiles for: {}",
                    players_[current_player_index_].name(), word);
      return false;
    }
  }
  return true;
}

bool Game::ExecutePlaceMove(const std::string& word, char dir, int row, int col,
                            Move& move) {
  bool horizontal = (dir == 'H' || dir == 'h');
  std::vector<int> tile_indices;
  if (!GetTileIndices(word, tile_indices)) {
    return false;
  }

  move.row = row;
  move.col = col;
  move.horizontal = horizontal;
  move.tile_indices = tile_indices;
  bool success = players_[current_player_index_].ExecutePlaceMove(
      bag_, dictionary_, board_, horizontal, row, col, tile_indices, move);
  if (success) {
    move_history_.push_back(move);
    NextTurn();
    spdlog::info("[Game] Player {} placed word: {} at ({}, {}), score: {}",
                 move.player_name, word, row, col, move.score);
  }
  return success;
}

bool Game::ExecuteMove(const std::string& move_type, const std::string& word,
                       char dir, int row, int col) {
  Move move;
  move.player_name = players_[current_player_index_].name();
  move.move_type = move_type;
  move.is_pass = false;

  std::string move_type_lower = move_type;
  std::transform(move_type_lower.begin(), move_type_lower.end(),
                 move_type_lower.begin(), ::tolower);

  if (move_type_lower == "pass") {
    ExecutePassMove();
    return true;
  } else if (move_type_lower == "place") {
    return ExecutePlaceMove(word, dir, row, col, move);
  } else if (move_type_lower == "swap") {
    return ExecuteSwapMove(word, move);
  }

  spdlog::error("[Game] Invalid move type: {}", move_type);
  return false;
}
}  // namespace core