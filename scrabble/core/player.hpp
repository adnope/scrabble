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

  void GetTiles(std::vector<Tile> &tiles);

  void UseTiles(char c);

  void ExchangeTile(char c, Bag &bag);

  bool FindTile(char c, int &pos);

  void ReturnTile(char c, std::vector<Tile> &used_tiles);

  void AddScore(const int score) { player_score_ += score; }

  void SubtractScore(const int score) { player_score_ -= score; }

  bool ExecutePlaceMove(Bag &bag, Dictionary &dictionary, Board &board,
                        char dir, int row, int col, const std::string &word);

  void ExecuteExchangeMove(Bag &bag, const std::string &word);

  int GetScore() const { return player_score_; }
  int GetHandSize() const { return static_cast<int>(playerTiles.size()); }
  std::string GetName() const { return player_name_; }
  int GetHandScore() const;
  std::vector<Tile> playerTiles;

 private:
  int player_score_;
  int num_tiles_;
  std::string player_name_;
};
}  // namespace core
