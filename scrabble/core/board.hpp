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

  std::vector<std::string> GetAllWords(int row, int col, char dir, int& score,
                                       const std::vector<Tile>& usedTiles);

  std::string GetVerticalWord(Tile tile, int row, int col, int& points);

  std::string GetHorizontalWord(Tile tile, int row, int col, int& points);

  void PlaceTile(const Tile& tile, const int row, const int col) {
    board_.at(row).at(col).PlaceTile(tile);
  }

  bool IsOccupiedAt(const int row, const int col) const {
    return board_.at(row).at(col).IsOccupied();
  }

  void set_first_move_status(const bool is_first_move) {
    is_first_move_ = is_first_move;
  }

  bool is_first_move() const { return is_first_move_; }

  const auto& board() { return board_; }

  static constexpr int kWidth = 15;
  static constexpr int kHeight = 15;
  static constexpr int kStartPosRow = 7;
  static constexpr int kStartPosColumn = 7;

 private:
  std::array<std::array<Square, kWidth>, kHeight> board_;

  bool is_first_move_;
};
}  // namespace core