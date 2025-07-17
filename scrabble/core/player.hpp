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

  void PutTilesInHand(const std::vector<Tile> &tiles);

  void UseTile(int tile_index);

  void SwapTile(Tile tile, Bag &bag);

  void ExecuteSwapMove(std::vector<Tile> &tiles, Bag &bag);

  bool FindTile(Tile tile, int &pos) const;

  void ReturnTile(Tile tile, std::vector<Tile> &used_tiles);

  void AddScore(const int score) { player_score_ += score; }

  void SubtractScore(const int score) { player_score_ -= score; }

  int GetHandScore() const;

  // Cần đổi sang vector<Tile> tiles thay vì word hay char
  bool ExecutePlaceMove(Bag &bag, const Dictionary &dictionary, Board &board,
                        bool horizontal, int row, int col,
                        const std::vector<Tile> &tiles);

  std::string name() const { return player_name_; }
  int score() const { return player_score_; }
  auto &player_tiles() { return player_tiles_; }
  int num_tiles_in_hand() const {
    return static_cast<int>(player_tiles_.size());
  }

 private:
  std::string player_name_;
  std::vector<Tile> player_tiles_;
  int player_score_;
  int num_tiles_;
};
}  // namespace core
