#pragma once

#include <vector>

#include "core/board.hpp"
#include "core/lexicon.hpp"
#include "core/tile.hpp"
#include "lexicon.hpp"

namespace core {
class Bot {
 public:
  Bot() = default;
  static constexpr int kMaxOffset = 7;
  static Board::Move FindBestMove(const std::vector<Tile>& rack, Board& board,
                                  const Lexicon& lexicon);

 private:
  static std::vector<Board::Move> GenerateAllMoves(
      const std::vector<Tile>& rack, Board& board);
  static bool IsAnchor(int row, int col, Board& board);
  static void GenerateMoveFromAnchor(int row, int col,
                                     const std::vector<Tile>& rack,
                                     bool horizontal,
                                     std::vector<Board::Move>& moves,
                                     Board& board);
  static bool IsValidMove(const Board::Move& move, Board& board);

  // Generate all subsets of tiles from the rack - tiles on hand
  static std::vector<std::vector<Tile>> TilesSubsets(
      const std::vector<Tile>& rack);
  static void GenerateSubsets(const std::vector<Tile>& rack, size_t index,
                              std::vector<Tile>& current,
                              std::vector<std::vector<Tile>>& result);

  // Generate all permutations of each subset of tiles
  static std::vector<std::vector<Tile>> GeneratePermutations(
      const std::vector<Tile>& tilesSubset, int length);

  static Board::Move CreateMove(const std::vector<Tile>& tiles, int row,
                                int col, bool horizontal, int offset);
};
};  // namespace core
