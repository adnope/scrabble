#pragma once

#include <array>
#include <string>
#include <vector>

#include "core/tile.hpp"
#include "lexicon.hpp"
#include "square.hpp"
#include "tile.hpp"
#include "word.hpp"

namespace core {
class Board {
 public:
  static constexpr int kWidth = 15;
  static constexpr int kHeight = 15;
  static constexpr int kStartPosRow = 7;
  static constexpr int kStartPosColumn = 7;

  enum class ResponseStatus : uint8_t {
    kSuccess,
    kNotInStartingSquare,
    kOccupied,
    kNotAligned,
    kNotAdjacent,
    kWordsInvalid
  };

  struct Placement {
    Tile tile;
    int row = 0;
    int col = 0;

    Placement(const Tile& tile, int row, int col)
        : tile(tile), row(row), col(col) {}
  };

  struct MoveValidationResponse {
    std::vector<Word> words;
    int move_points;
    ResponseStatus status;
    std::string invalid_word;
  };

  using Move = std::vector<Placement>;
  using BoardGrid = std::array<std::array<Square, kHeight>, kWidth>;

  Board();

  bool PlaceTile(const Tile& tile, int row, int col);

  bool IsOccupied(const int row, const int col) const {
    if (row < 0 || row >= kHeight || col < 0 || col >= kWidth) {
    return false;
    }
    return board_grid_.at(row).at(col).IsOccupied();
  }

  MoveValidationResponse ValidateMove(const Move& move, const Lexicon& lexicon);

  std::array<std::array<std::string, 15>, 15> GetDisplayFormat() const;

  bool is_first_move() const { return is_first_move_; }

  void SetFirstMoveStatus(const bool is_first_move) {
    is_first_move_ = is_first_move;
  }

  bool IsMoveOccupied(const Move& move) const;

  Word GetWordFromPos(int row, int col, bool horizontal, const Move& move);

  std::vector<Word> GetWordsFromMove(const Move& move, bool horizontal);

  Tile GetTile(int row, int col) const;
  Tile GetTile(int case_curr) const;

 private:
  BoardGrid board_grid_;
  bool is_first_move_ = true;

  static bool AdjacentToAnotherInMove(int row, int col, const Move& move);

  bool AdjacentToBoard(int row, int col) const;

  bool IsAdjacentToExistingTiles(const Move& move) const;

  static bool IsInStartingSquare(const Move& move);

  bool IsInStartingSquare(const Move& move) const;

  static int IsAligned(const Move& move);

  static bool AreInDictionary(const std::vector<std::string>& words,
                              const Lexicon& lexicon,
                              std::string& invalid_word);
};
}  // namespace core
