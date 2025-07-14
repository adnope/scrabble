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

  void GetTiles(Bag &bag, int number_of_tiles);

  void UseTiles(char c);

  void ExchangeTile(char c, Bag &bag);

  bool FindTile(char c, int &pos);

  void ReturnTile(char c, std::vector<Tile> &used_tiles);

  void AddScore(const int score) { player_score += score; }

  void SubtractScore(const int score) { player_score -= score; }

  bool ExecutePlaceMove(Bag &bag, Dictionary &dictionary, Board &board,
                        char dir, int row, int col, const std::string &word);

  void ExecuteExchangeMove(Bag &bag, const std::string &word);

  int GetScore() const { return player_score; }
  int GetHandSize() const { return static_cast<int>(hand_tiles.size()); }
  std::string GetName() const { return player_name; }
  int GetHandScore() const;

 private:
  int player_score;
  int num_tiles;
  std::string player_name;
  std::vector<Tile> hand_tiles;
};
}  // namespace core
