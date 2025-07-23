#include "game.hpp"

#include <iostream>
#include <string>

#include "core/board.hpp"
#include "spdlog/spdlog.h"

namespace core {
Game::Game(Dictionary::DictionaryType dict_type) {
  dictionary_ = Dictionary(dict_type);
}

bool Game::IsGameOver() {
  if (bag_.num_tiles_remanining() == 0) {
    for (const auto& player : players_) {
      if (player.current_deck_size() == 0) {
        EndGame();
        return true;
      }
    }
  } else if (consecutive_passes_ >= 2 * NumPlayers()) {
    EndGame();
    return true;
  }
  return false;
}

void Game::NextTurn() {
  current_player().DrawNewTiles(bag_);
  current_player_index_ = (current_player_index_ + 1) % NumPlayers();
}

void Game::ExecutePassMove() {
  Move move;
  move.type = MoveType::kPassing;
  move.player_name = current_player().name();
  move_history_.push_back(move);

  NextTurn();
  ++consecutive_passes_;
}

bool Game::ExecuteSwapMove(const std::vector<int>& indices) {
  bool swapped = current_player().PerformSwap(bag_, indices);
  if (!swapped) {
    std::cout << "Not enough tiles in bag to swap!\n";
  } else {
    Move move;
    move.type = MoveType::kSwapping;
    move.player_name = GetCurrentPlayer().name();
    move_history_.push_back(move);

    NextTurn();
    consecutive_passes_ = 0;
  }
  return swapped;
}

void Game::EndGame() {
  int winner_index = 0;
  int highest_score = players_[0].score();
  for (int i = 0; i < NumPlayers(); ++i) {
    if (players_[i].score() > highest_score) {
      highest_score = players_[i].score();
      winner_index = i;
    }
  }

  for (int i = 0; i < NumPlayers(); ++i) {
    if (i != winner_index) {
      players_[winner_index].AddScore(players_[i].GetDeckScore());
      players_[i].SubtractScore(players_[i].GetDeckScore());
    }
  }

  winner_ = players_[winner_index];

  spdlog::info("[Game] Game ended. Winner: {}", winner_.name());
}

Board::ResponseStatus Game::ExecutePlaceMove(const Player::Move& player_move) {
  const auto player_response =
      current_player().SubmitMove(player_move, board_, dictionary_);
  if (player_response.status == Board::ResponseStatus::kSuccess) {
    Move move;
    move.type = MoveType::kPlacing;
    move.player_name = current_player().name();
    move.points = player_response.move_points;
    move.words = player_response.words;
    move_history_.push_back(move);
    NextTurn();
  }

  return player_response.status;
}
}  // namespace core