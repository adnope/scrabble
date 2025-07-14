#pragma once

#include <array>
#include <string>
#include <vector>

#include "square.hpp"
#include "tile.hpp"

namespace core {
class Board {
 public:
  explicit Board();

  std::vector<std::string> AllWords(int row, int col, char dir, int& score,
                                    std::vector<Tile>& usedTiles);

  std::string VerticalWord(Tile t, int row, int col, int& points);
  std::string HorizontalWord(Tile t, int row, int col, int& points);

  void PlaceTile(const Tile& tile, const int row, const int col) {
    board_.at(row).at(col).PlaceTile(tile);
  }

  bool IsOccupiedAt(const int row, const int col) const {
    return board_.at(row).at(col).IsOccupied();
  }

  void SetFirstMove(const bool is_first_move) {
    is_first_move_ = is_first_move;
  }
  bool GetFirstMove() const { return is_first_move_; }

  const auto& GetBoard() { return board_; }

  static constexpr int WIDTH = 15;
  static constexpr int HEIGHT = 15;
  static constexpr int START_POS_X = 7;
  static constexpr int START_POS_Y = 7;

 private:
  std::array<std::array<Square, WIDTH>, HEIGHT> board_;

  bool is_first_move_;
};
}  // namespace core