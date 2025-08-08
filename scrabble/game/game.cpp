#include "game.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "core/board.hpp"
#include "core/tile.hpp"

namespace game {
Game::Game(const std::vector<std::string>& player_names) {
  for (const auto& name : player_names) {
    AddPlayer(name);
  }
  // const core::Board::Move initial_board = {
  //     {{'V', 4}, 5, 7},  {{'I', 1}, 5, 8}, {{'S', 1}, 5, 9}, {{'I', 1}, 5,
  //     10},
  //     {{'T', 1}, 5, 11}, {{'T', 1}, 7, 4}, {{'A', 1}, 7, 5}, {{'N', 1}, 7,
  //     6},
  //     {{'N', 1}, 7, 7},  {{'E', 1}, 7, 8}, {{'D', 2}, 7, 9}, {{'P', 3}, 2,
  //     8},
  //     {{'R', 1}, 3, 8},  {{'A', 1}, 4, 8}, {{'S', 1}, 6, 8}};
  // for (const auto& placement : initial_board) {
  //   board_.PlaceTile(placement.tile, placement.row, placement.col);
  //   bag_.RemoveTile(placement.tile.letter());
  // }
}

void Game::InitPlayerDecks() {
  for (auto& player : players_) {
    player.DrawNewTiles(bag_);
  }
}

bool Game::IsOver() {
  if (!is_over) {
    if (bag_.num_tiles_remanining() == 0) {
      for (const auto& player : players_) {
        if (player.current_deck_size() == 0) {
          EndGame();
          is_over = true;
        }
      }
    } else if (consecutive_passes_ >= 2 * num_players()) {
      EndGame();
      is_over = true;
    }
  }
  return is_over;
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
  ++turn_number_;
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
    ++turn_number_;
  }
  return {swap_successful, old_tiles, new_tiles, deck};
}

void Game::EndGame() {
  bool end_by_passing = consecutive_passes_ >= 2 * num_players();
  if (end_by_passing) {
    for (int i = 0; i < num_players(); ++i) {
      players_[i].SubtractScore(players_[i].GetDeckScore());
    }
    auto it = std::max_element(
        players_.begin(), players_.end(),
        [](const auto& a, const auto& b) { return a.score() < b.score(); });
    winner_ = players_[static_cast<int>(std::distance(players_.begin(), it))];
  } else {
    int game_ender_index = 0;
    for (int i = 0; i < num_players(); ++i) {
      if (players_[i].current_deck_size() == 0) {
        game_ender_index = i;
        break;
      }
    }
    for (int i = 0; i < num_players() && i != game_ender_index; ++i) {
      players_[game_ender_index].AddScore(players_[i].GetDeckScore());
      players_[i].SubtractScore(players_[i].GetDeckScore());
    }
    auto it = std::max_element(
        players_.begin(), players_.end(),
        [](const auto& a, const auto& b) { return a.score() < b.score(); });
    winner_ = players_[static_cast<int>(std::distance(players_.begin(), it))];
  }
  std::cout << "Winner: " << winner_.name() << "\nScore: " << winner_.score()
            << '\n';
}

core::Player::MoveSubmissionResponse Game::ExecutePlaceMove(
    const core::Player::Move& player_move) {
  const auto player_response =
      current_player_ref().SubmitMove(player_move, board_, *lexicon_);
  if (player_response.status == core::Board::ResponseStatus::kSuccess) {
    Move move;
    move.type = MoveType::kPlacing;
    move.player_name = current_player_ref().name();
    move.points = player_response.move_points;
    move.words = player_response.words;
    move_history_.push_back(move);
    NextTurn();
    ++turn_number_;
  }

  return player_response;
}

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