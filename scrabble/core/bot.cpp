#include "bot.hpp"

#include <algorithm>
#include <cstddef>
#include <vector>

#include "core/board.hpp"
#include "core/lexicon.hpp"
#include "core/tile.hpp"

namespace core {
Board::Move Bot::FindBestMove(const std::vector<Tile>& rack, Board& board,
                              const Lexicon& lexicon) {
  std::vector<Board::Move> allMoves = GenerateAllMoves(rack, board);
  //Board::Move bestMove;
  //Init best_Move, maxScore
  auto best_Move = allMoves[0];
  auto maxScore = board.ValidateMove(best_Move, lexicon).move_points;
  //int maxScore = -1;
  for (const auto& move : allMoves) {
    auto [words, score, status] = board.ValidateMove(move, lexicon);
    if (status == Board::ResponseStatus::kSuccess && score > maxScore) {
      maxScore = score;
      best_Move = move;
    }
  }

  return best_Move;
}
std::vector<Board::Move> Bot::GenerateAllMoves(const std::vector<Tile>& rack,
                                               Board& board) {
  std::vector<Board::Move> allMoves;

  for (int row = 0; row < Board::kHeight; row++) {
    for (int col = 0; col < Board::kWidth; col++) {
      if (IsAnchor(row, col, board)) {
        // Generate all possible moves for this anchor
        GenerateMoveFromAnchor(row, col, rack, true, allMoves, board);
        GenerateMoveFromAnchor(row, col, rack, false, allMoves, board);
      }
    }
  }
  return allMoves;
}

bool Bot::IsAnchor(int row, int col, Board& board) {
  if (board.IsOccupied(row, col)) {
    return false;
  }
  return (row > 0 && board.IsOccupied(row - 1, col)) ||
         (row < Board::kHeight - 1 && board.IsOccupied(row + 1, col)) ||
         (col > 0 && board.IsOccupied(row, col - 1)) ||
         (col < Board::kWidth - 1 && board.IsOccupied(row, col + 1)) ||
         (row == Board::kStartPosRow && col == Board::kStartPosColumn);
}

void Bot::GenerateMoveFromAnchor(int row, int col,
                                 const std::vector<Tile>& rack, bool horizontal,
                                 std::vector<Board::Move>& moves,
                                 Board& board) {
  std::vector<std::vector<Tile>> subsets = TilesSubsets(rack);
  for (const auto& subset : subsets) {
    if (subset.empty()) {
      continue;
    }
    for (int len = 1; len <= static_cast<int>(subset.size()); len++) {
      std::vector<std::vector<Tile>> permutations =
          GeneratePermutations(subset, len);
      for (const auto& permutation : permutations) {
        for (int offset = -Bot::kMaxOffset; offset <= Bot::kMaxOffset;
             offset++) {
          Board::Move move =
              CreateMove(permutation, row, col, horizontal, offset);
          if (!move.empty() && IsValidMove(move, board)) {
            moves.push_back(move);
          }
        }
      }
    }
  }
}

bool Bot::IsValidMove(const Board::Move& move, Board& board) {
  if (move.empty()) {
    return false;
  }
  bool connected = false;
  for (const auto& [tile, row, col] : move) {
    if (row > 0 && board.IsOccupied(row - 1, col)) {
      connected = true;
    }
    if (row < Board::kHeight - 1 && board.IsOccupied(row + 1, col)) {
      connected = true;
    }
    if (col > 0 && board.IsOccupied(row, col - 1)) {
      connected = true;
    }
    if (col < Board::kWidth - 1 && board.IsOccupied(row, col + 1)) {
      connected = true;
    }
    if (row == Board::kStartPosRow && col == Board::kStartPosColumn) {
      connected = true;
    }
  }
  return connected && !board.IsMoveOccupied(move);
}

std::vector<std::vector<Tile>> Bot::TilesSubsets(
    const std::vector<Tile>& rack) {
  std::vector<std::vector<Tile>> result;
  std::vector<Tile> current;

  GenerateSubsets(rack, 0, current, result);
  return result;
}

void Bot::GenerateSubsets(const std::vector<Tile>& rack, size_t index,
                         std::vector<Tile>& current,
                         std::vector<std::vector<Tile>>& result) {
  // Thêm tập con hiện tại vào result
  if (!current.empty()) {
    result.push_back(current);
  }

  for (size_t i = index; i < rack.size(); ++i) {
    // Blank tile
    if (rack[i].letter() == '?') {
      for (char letter = 'A'; letter <= 'Z'; ++letter) {
        current.push_back(Tile(letter, 0)); 
        GenerateSubsets(rack, i + 1, current, result);
        current.pop_back(); // Quay lui
      }
    } else {
      // No blank tiles
      current.push_back(rack[i]);
      GenerateSubsets(rack, i + 1, current, result);
      current.pop_back();
    }
  }
}


std::vector<std::vector<Tile>> Bot::GeneratePermutations(
    const std::vector<Tile>& tilesSubset, int length) {
  std::vector<std::vector<Tile>> result;
  std::vector<Tile> current = tilesSubset;
  std::sort(current.begin(), current.end());

  while (std::next_permutation(current.begin(), current.end())) {
    std::vector<Tile> perm(current.begin(), current.end() + length);
    result.push_back(perm);
  }
  return result;
}


Board::Move Bot::CreateMove(const std::vector<Tile>& tiles, int row, int col,
                            bool horizontal, int offset) {
  Board::Move move;
  int start_row = row;
  int start_col = col;

  if (horizontal) {
    start_col += offset;
  } else {
    start_row += offset;
  }

  for (size_t i = 0; i < tiles.size(); i++) {
    int r = horizontal ? start_row : start_row + static_cast<int>(i);
    int c = horizontal ? start_col + static_cast<int>(i) : start_col;

    if (r < 0 || r >= Board::kHeight || c < 0 || c >= Board::kWidth) {
      return {};
    }
  }
  return move;
}

};  // namespace core
