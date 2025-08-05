#include "game.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "core/board.hpp"
#include "core/tile.hpp"

namespace game {
void Game::LoadDictionary(core::Dictionary::DictionaryType type) {
  lexicon_.PreloadDictionary(type);
}

void Game::InitPlayerDecks() {
  for (auto& player : players_) {
    player.DrawNewTiles(bag_);
  }
}

bool Game::IsOver() {
  if (bag_.num_tiles_remanining() == 0) {
    for (const auto& player : players_) {
      if (player.current_deck_size() == 0) {
        EndGame();
        return true;
      }
    }
  } else if (consecutive_passes_ >= 2 * num_players()) {
    EndGame();
    return true;
  }
  return false;
}

void Game::NextTurn() {
  current_player_ref().DrawNewTiles(bag_);
  current_player_index_ = (current_player_index_ + 1) % num_players();
}

void Game::ExecutePassMove() {
  Move move;
  move.type = MoveType::kPassing;
  move.player_name = current_player_ref().name();
  move_history_.push_back(move);

  NextTurn();
  ++consecutive_passes_;
}

Game::SwapResponse Game::ExecuteSwapMove(const std::vector<int>& indices) {
  std::vector<std::string> old_tiles;
  for (const int index : indices) {
    const char letter = current_player_ref().deck().at(index).letter();
    const int points = current_player_ref().deck().at(index).points();
    std::string tile_str;
    tile_str += letter + std::to_string(points);
    old_tiles.push_back(tile_str);
  }
  bool swap_successful = current_player_ref().PerformSwap(bag_, indices);
  std::vector<std::string> new_tiles;
  for (const int index : indices) {
    const char letter = current_player_ref().deck().at(index).letter();
    const int points = current_player_ref().deck().at(index).points();
    std::string tile_str;
    tile_str += letter + std::to_string(points);
    new_tiles.push_back(tile_str);
  }
  std::string deck;
  for (const core::Tile& tile : current_player_ref().deck()) {
    deck += tile.letter() + std::to_string(tile.points()) + ' ';
  }
  if (swap_successful) {
    Move move;
    move.type = MoveType::kSwapping;
    move.player_name = current_player_ref().name();
    move_history_.push_back(move);

    NextTurn();
    consecutive_passes_ = 0;
  }
  return {swap_successful, old_tiles, new_tiles, deck};
}

void Game::EndGame() {
  int winner_index = 0;
  int highest_score = players_[0].score();
  for (int i = 0; i < num_players(); ++i) {
    if (players_[i].score() > highest_score) {
      highest_score = players_[i].score();
      winner_index = i;
    }
  }

  for (int i = 0; i < num_players(); ++i) {
    if (i != winner_index) {
      players_[winner_index].AddScore(players_[i].GetDeckScore());
      players_[i].SubtractScore(players_[i].GetDeckScore());
    }
  }

  winner_ = players_[winner_index];
}

core::Board::ResponseStatus Game::ExecutePlaceMove(
    const core::Player::Move& player_move) {
  const auto player_response =
      current_player_ref().SubmitMove(player_move, board_, lexicon_);
  if (player_response.status == core::Board::ResponseStatus::kSuccess) {
    Move move;
    move.type = MoveType::kPlacing;
    move.player_name = current_player_ref().name();
    move.points = player_response.move_points;
    move.words = player_response.words;
    move_history_.push_back(move);
    NextTurn();
  }

  return player_response.status;
}

void Game::PrintBoard() const { std::cout << board_.GetDisplayFormat(); }

void Game::PrintDebugInfo() const {
  std::cout << "[DEBUG]: GAME:\n";
  std::cout << "[Players]:\n";
  for (const auto& player : players_) {
    std::cout << "Name: " << player.name() << '\n';
    std::cout << "Score: " << player.score() << '\n';
    std::cout << "Deck: ";
    player.PrintDeck();
    std::cout << "Deck score: " << player.GetDeckScore() << '\n';
    std::cout << "Deck size: " << player.current_deck_size() << '\n';
  }
  bag_.PrintBagInfo();
  // std::cout << board_.GetDisplayFormat();

  std::cout << "current_player_index: " << current_player_index_ << '\n';
  std::cout << "consecutive_passes: " << consecutive_passes_ << '\n';
}
}  // namespace game