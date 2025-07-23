#pragma once

#include <array>
#include <string>
#include <vector>

#include "core/dictionary.hpp"
#include "square.hpp"
#include "tile.hpp"

namespace core {
class Board {
 public:
  static constexpr int kWidth = 15;
  static constexpr int kHeight = 15;
  static constexpr int kStartPosRow = 7;
  static constexpr int kStartPosColumn = 7;

  struct Placement {
    Tile tile;
    int row = 0;
    int col = 0;
  };

  struct Word {
    std::string content;
    int points = 0;
    int multiplier = 1;

    void ApplyMultiplier() { points *= multiplier; }
  };

  Board();

  bool IsOccupied(const int row, const int col) const {
    return board_grid_.at(row).at(col).IsOccupied();
  }

  bool PlaceTile(const Tile& tile, int row, int col);

  std::vector<Word> GetWordsFromMove(const std::vector<Placement>& move,
                                     bool horizontal);

  int PlaceMoveAndGetScore(const std::vector<Placement>& move,
                           const Dictionary& dictionary);

  std::string GetDisplayFormat();

  bool is_first_move() const { return is_first_move_; }
  void SetFirstMoveStatus(const bool is_first_move) {
    is_first_move_ = is_first_move;
  }

 private:
  std::array<std::array<Square, kWidth>, kHeight> board_grid_;
  bool is_first_move_;

  bool IsMoveOccupied(const std::vector<Placement>& move) const;

  Word GetWordFromPos(int row, int col, bool horizontal,
                      const std::vector<Placement>& move);

  static int IsAligned(const std::vector<Placement>& move);

  static bool AreInDictionary(const std::vector<std::string>& words,
                              const Dictionary& dictionary);
};
}  // namespace core