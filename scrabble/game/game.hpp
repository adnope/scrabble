#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "core/bag.hpp"
#include "core/board.hpp"
#include "core/lexicon.hpp"
#include "core/player.hpp"
#include "core/word.hpp"

namespace game {
class Game {
 public:
  enum class MoveType : uint8_t { kPlacing, kSwapping, kPassing };

  struct Move {
    MoveType type;
    std::string player_name;
    std::vector<core::Word> words;
    int points = 0;
  };

  struct SwapResponse {
    bool sucessful;
    std::vector<std::string> old_tiles;
    std::vector<std::string> new_tiles;
    std::string deck;
  };

  Game() {}

  explicit Game(const std::vector<std::string>& player_names);

  void AddPlayer(const std::string& name) { players_.emplace_back(name, 0); }

  void InitPlayerDecks();

  void SetFirstPlayer(const int index) { current_player_index_ = index; }
  void SetLexicon(core::Lexicon* lexicon) { lexicon_ = lexicon; }

  void ExecutePassMove();

  SwapResponse ExecuteSwapMove(const std::vector<int>& indices);

  core::Player::MoveSubmissionResponse ExecutePlaceMove(
      const core::Player::Move& player_move);

  void NextTurn();

  bool IsOver();

  core::Bag bag() const { return bag_; }
  core::Board board() const { return board_; }
  std::vector<core::Player> players() const { return players_; }
  int consecutive_passes() const { return consecutive_passes_; }
  std::vector<Move> move_history() const { return move_history_; }
  int turn_number() const { return static_cast<int>(turn_number_); }
  int bag_size() const { return bag_.num_tiles_remanining(); }
  core::Player winner() const { return winner_; }
  int num_players() const { return static_cast<int>(players_.size()); }
  core::Player current_player() const {
    return players_[current_player_index_];
  }
  core::Lexicon* lexicon() { return lexicon_; }

  void PrintBoard() const;
  void PrintDebugInfo() const;

 private:
  std::vector<core::Player> players_;
  core::Bag bag_;
  core::Board board_;
  core::Lexicon* lexicon_ = nullptr;
  std::vector<Move> move_history_;
  unsigned int turn_number_ = 0;
  bool is_over = false;

  int current_player_index_ = 0;
  int consecutive_passes_ = 0;
  core::Player winner_{"", 0};

  void EndGame();
  core::Player& current_player_ref() { return players_[current_player_index_]; }
};
}  // namespace game