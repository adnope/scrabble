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
    board.at(row).at(col).PlaceTile(tile);
  }

  bool IsOccupiedAt(const int row, const int col) const {
    return board.at(row).at(col).IsOccupied();
  }

  void SetFirstMove(const bool b) { is_first_move = b; }
  bool GetFirstMove() const { return is_first_move; }

  const auto& GetBoard() { return board; }

  // int getRowSize() const;
  // int getColSize() const;
  // int getStartX() const;
  // int getStartY() const;

  static constexpr int WIDTH = 15;
  static constexpr int HEIGHT = 15;
  static constexpr int START_POS_X = 7;
  static constexpr int START_POS_Y = 7;

 private:
  // std::vector<std::vector<Square> > boardVect;
  std::array<std::array<Square, WIDTH>, HEIGHT> board;

  bool is_first_move;
};
}  // namespace core