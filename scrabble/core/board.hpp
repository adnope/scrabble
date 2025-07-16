#pragma once

#include <array>
#include <string>
#include <vector>

#include "square.hpp"
#include "tile.hpp"

namespace core {
class Board {
 public:
  struct PlayerMove {
    Tile tile;
    int row = 0;
    int col = 0;
  };

  Board();

  std::string GetVerticalWord(Tile tile, int row, int col, int& points);

  std::string GetHorizontalWord(Tile tile, int row, int col, int& points);

  std::vector<std::string> GetAllWords(int row, int col, bool horizontal,
                                       int& score,
                                       const std::vector<Tile>& usedTiles);

  void PlaceTile(const Tile& tile, const int row, const int col) {
    board_grid_.at(row).at(col).PlaceTile(tile);
  }

  bool IsOccupiedAt(const int row, const int col) const {
    return board_grid_.at(row).at(col).IsOccupied();
  }

  bool is_first_move() const { return is_first_move_; }
  void SetFirstMoveStatus(const bool is_first_move) {
    is_first_move_ = is_first_move;
  }

  static constexpr int kWidth = 15;
  static constexpr int kHeight = 15;
  static constexpr int kStartPosRow = 7;
  static constexpr int kStartPosColumn = 7;

 private:
  std::array<std::array<Square, kWidth>, kHeight> board_grid_;
  bool is_first_move_;
};
}  // namespace core