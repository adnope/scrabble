#pragma once

#include <map>
#include <vector>

#include "core/board.hpp"
#include "core/lexicon.hpp"
#include "core/tile.hpp"
#include "lexicon.hpp"

namespace core {

struct MoveGenState {
  std::map<char, int> freq;
  std::string mot;
  bool orientation;
  bool plus;
  std::vector<Tile> rack;

  MoveGenState(std::map<char, int>&& f, std::string&& m, bool o, bool p,
               std::vector<Tile> r)
      : freq(std::move(f)),
        mot(std::move(m)),
        orientation(o),
        plus(p),
        rack(std::move(r)) {}
};

class Bot {
 public:
  Bot() {};
  static constexpr int kMaxOffset = 7;
  static Board::Move FindBestMove(const std::vector<Tile>& rack, Board& board,
                                  Lexicon& lexicon);
  void addMove(Board::Move& move);
  std::vector<Board::Move> getMoves() const;

 private:
  static std::vector<Board::Move> GenerateAllMoves(
      const std::vector<Tile>& rack, Board& board, Lexicon& lexicon);
  static bool IsAnchor(int row, int col, Board& board);
  static void GenerateMoveFromAnchor(int row, int col, bool horizontal,
                                     const std::vector<Tile>& rack,
                                     Board& board, Lexicon& lexicon,
                                     Board::Move& move,
                                     std::vector<Board::Move>& moves);
  bool IsValidMove(const Board::Move& move, Board& board) const;
  static Board::Move CreateMove();
  static bool VerifyCrossWords(int case_curr, char c, Board& board,
                               Lexicon& lexicon, const Board::Move& move);
  static void GenerateWords(Node* node, MoveGenState& state, int case_curr,
                            Board& board, Lexicon& lexicon,
                            Board::Move& currentMove, std::vector<Board::Move> moves);
  std::vector<Board::Move> moves;
  static void ProcessOccupiedCell(Node* node, MoveGenState& state,
                                  int case_curr, Board& board,
                                  Lexicon& lexicon,
                                  Board::Move& currentMove,
                                  std::vector<Board::Move>& moves);
  static void ProcessEmptyCell(Node* node, MoveGenState& state,
                               int case_curr, Board& board,
                               Lexicon& lexicon, Board::Move& currentMove,
                               std::vector<Board::Move>& moves);
  static void GenerateNextMove(Node* node, MoveGenState& state,
                               int next_case, Board& board,
                               Lexicon& lexicon, Board::Move& currentMove,
                               std::vector<Board::Move>& moves);
};

};  // namespace core
