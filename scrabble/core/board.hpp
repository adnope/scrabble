#pragma once

#include <array>
#include <string>
#include <vector>

#include "dictionary.hpp"
#include "square.hpp"
#include "tile.hpp"
#include "word.hpp"

namespace core {
/**
  Class usage:
  - Init using default constructor
  - Use the SubmitMove() method to get a response
  - Response contains:
    + words: a list of Word objects. Use the content() method to receive and
    display the content of the word
    + move_points: total points of the move
    + status code: 0 means success, -1 means occupied, -2 means moves not
    aligned, -3 means words not in dictionary
*/
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

  struct MoveSubmissionResponse {
    std::vector<Word> words;
    int move_points;
    int status_code;
  };

  using Move = std::vector<Placement>;
  using BoardGrid = std::array<std::array<Square, kHeight>, kWidth>;

  Board();

  bool PlaceTile(const Tile& tile, int row, int col);

  bool IsOccupied(const int row, const int col) const {
    return board_grid_.at(row).at(col).IsOccupied();
  }

  MoveSubmissionResponse SubmitMove(const Move& move,
                                    const Dictionary& dictionary);

  std::string GetDisplayFormat();

  bool is_first_move() const { return is_first_move_; }

  void SetFirstMoveStatus(const bool is_first_move) {
    is_first_move_ = is_first_move;
  }

 private:
  BoardGrid board_grid_;
  bool is_first_move_;

  bool IsMoveOccupied(const Move& move) const;

  Word GetWordFromPos(int row, int col, bool horizontal, const Move& move);

  std::vector<Word> GetWordsFromMove(const Move& move, bool horizontal);

  static int IsAligned(const Move& move);

  static bool AreInDictionary(const std::vector<std::string>& words,
                              const Dictionary& dictionary);
};
}  // namespace core