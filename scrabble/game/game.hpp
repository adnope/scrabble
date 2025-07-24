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

namespace core {
class Game {
 public:
  enum class MoveType : uint8_t { kPlacing, kSwapping, kPassing };

  struct Move {
    MoveType type;
    std::string player_name;
    std::vector<Word> words;
    int points = 0;
  };

  explicit Game(Dictionary::DictionaryType dict_type);

  void AddPlayer(const std::string& name, const int score) {
    players_.emplace_back(name, score);
  }

  int NumPlayers() const { return static_cast<int>(players_.size()); }

  bool IsGameOver();

  void NextTurn();

  void ExecutePassMove();

  bool ExecuteSwapMove(const std::vector<int>& indices);

  Board::ResponseStatus ExecutePlaceMove(const Player::Move& player_move);

  void Start() {
    
  }

  Player GetCurrentPlayer() const { return players_[current_player_index_]; }

  int GetBagSize() const { return bag_.num_tiles_remanining(); }

  Player GetWinner() const { return winner_; }

  std::vector<Move> GetMoveHistory() const { return move_history_; }

 private:
  std::vector<Player> players_;
  Bag bag_;
  Board board_;
  Dictionary dictionary_;
  Lexicon lexicon_;
  std::vector<Move> move_history_;

  int current_player_index_ = 0;
  int consecutive_passes_ = 0;
  Player winner_{"", 0};

  void EndGame();
  Player& current_player() {
    return players_[current_player_index_];
  }
};
}  // namespace core