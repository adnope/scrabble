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

  void UseTile(int index);

  void PutTileToUsedTiles(int index, std::vector<Tile> &used_tiles);

  void AddScore(const int score) { player_score_ += score; }

  void SubtractScore(const int score) { player_score_ -= score; }

  int GetHandScore() const;

  bool ExecutePlaceMove(Bag &bag, const Dictionary &dictionary, Board &board,
                        bool horizontal, int row, int col,
                        const std::vector<int>& tile_indices);

  bool PerformSwap(Bag &bag, const std::vector<int>& indices);

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

  void SwapTile(int index, Bag &bag);
};
}  // namespace core