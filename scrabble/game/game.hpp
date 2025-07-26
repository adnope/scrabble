#pragma once

#include <cstdint>
#include <vector>

#include "core/bag.hpp"
#include "core/board.hpp"
#include "core/dictionary.hpp"
#include "core/lexicon.hpp"
#include "core/player.hpp"
#include "core/word.hpp"
#include "core/lexicon.hpp"

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

  explicit Game(core::Dictionary::DictionaryType dict_type);

  void AddPlayer(const std::string& name) { players_.emplace_back(name, 0); }

  int NumPlayers() const { return static_cast<int>(players_.size()); }

  void InitPlayerDecks();

  bool IsGameOver();

  void NextTurn();

  void ExecutePassMove();

  bool ExecuteSwapMove(const std::vector<int>& indices);

  core::Board::ResponseStatus ExecutePlaceMove(
      const core::Player::Move& player_move);

  core::Player GetCurrentPlayer() const {
    return players_[current_player_index_];
  }

  int GetBagSize() const { return bag_.num_tiles_remanining(); }

  core::Player GetWinner() const { return winner_; }

  std::vector<Move> GetMoveHistory() const { return move_history_; }

  void SetFirstPlayer(const int index) { current_player_index_ = index; }

  core::Bag bag() const { return bag_; }

  std::vector<core::Player> players() const { return players_; }

  void PrintDebugInfo();

 private:
  std::vector<core::Player> players_;
  core::Bag bag_;
  core::Board board_;
  core::Dictionary dictionary_;
  core::Lexicon lexicon_;
  std::vector<Move> move_history_;

  int current_player_index_ = 0;
  int consecutive_passes_ = 0;
  core::Player winner_{"", 0};

  void EndGame();
  core::Player& current_player() { return players_[current_player_index_]; }
};
}  // namespace game