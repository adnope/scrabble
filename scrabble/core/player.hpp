#pragma once
#include <string>
#include <vector>

#include "bag.hpp"
#include "board.hpp"
#include "dictionary.hpp"
#include "tile.hpp"

namespace core {
class Player {
 public:
  Player(const std::string &player_name, int score, int number_of_tiles);

  void GetTiles(const std::vector<Tile> &tiles);

  void UseTiles(char c);

  void ExchangeTile(char c, Bag &bag);

  bool FindTile(char c, int &pos) const;

  void ReturnTile(char c, std::vector<Tile> &used_tiles);

  void AddScore(const int score) { player_score_ += score; }

  void SubtractScore(const int score) { player_score_ -= score; }

  bool ExecutePlaceMove(Bag &bag, const Dictionary &dictionary, Board &board,
                        Direction direction, int row, int col,
                        const std::string &word);

  void ExecuteExchangeMove(Bag &bag, const std::string &word);

  int score() const { return player_score_; }
  int num_tiles_in_hand() const { return static_cast<int>(player_tiles_.size()); }
  std::string name() const { return player_name_; }

 private:
  int player_score_;
  int num_tiles_;
  std::string player_name_;
  std::vector<Tile> player_tiles_;
};
}  // namespace core
