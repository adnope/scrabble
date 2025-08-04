#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "core/bag.hpp"
#include "core/board.hpp"
#include "core/dictionary.hpp"
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

  Game(core::Dictionary::DictionaryType dict_type,
       const std::vector<std::string>& player_names);

  void LoadDictionary(core::Dictionary::DictionaryType type);

  void AddPlayer(const std::string& name) { players_.emplace_back(name, 0); }

  void InitPlayerDecks();

  void SetFirstPlayer(const int index) { current_player_index_ = index; }

  void ExecutePassMove();

  SwapResponse ExecuteSwapMove(const std::vector<int>& indices);

  core::Board::ResponseStatus ExecutePlaceMove(
      const core::Player::Move& player_move);

  void NextTurn();

  bool IsOver();

  core::Bag bag() const { return bag_; }
  core::Board board() const { return board_; }
  std::vector<core::Player> players() const { return players_; }
  int consecutive_passes() const { return consecutive_passes_; }
  std::vector<Move> move_history() const { return move_history_; }
  int bag_size() const { return bag_.num_tiles_remanining(); }
  core::Player winner() const { return winner_; }
  int num_players() const { return static_cast<int>(players_.size()); }
  core::Player current_player() const {
    return players_[current_player_index_];
  }

  void PrintBoard() const;
  void PrintDebugInfo() const;

 private:
  std::vector<core::Player> players_;
  core::Bag bag_;
  core::Board board_;
  core::Lexicon lexicon_;
  std::vector<Move> move_history_;

  int current_player_index_ = 0;
  int consecutive_passes_ = 0;
  core::Player winner_{"", 0};

  void EndGame();
  core::Player& current_player_ref() { return players_[current_player_index_]; }
};
}  // namespace game
