#pragma once

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "bag.hpp"
#include "core/board.hpp"
#include "core/lexicon.hpp"
#include "core/tile.hpp"
#include "player.hpp"
namespace core {
struct MoveGenState {
  std::map<char, int> freq;  // Tần suất chữ cái
  std::string mot;           // Từ đang xây dựng
  bool orientation;          // Hướng: true (ngang), false (dọc)
  bool plus;                 // Hướng di chuyển trong GADDAG
  std::vector<Tile> rack;    // Các Tile trên tay
  std::vector<int> tile_indices;
  Player::Move current_move;  // Nước đi hiện tại

  MoveGenState(std::map<char, int>&& f, std::string&& m, bool o, bool p,
               std::vector<Tile> r, std::vector<int> idx)
      : freq(std::move(f)),
        mot(std::move(m)),
        orientation(o),
        plus(p),
        rack(std::move(r)),
        tile_indices(std::move(idx)) {}
};

class Bot : public Player {
 public:
  explicit Bot(const std::string& player_name, int score = 0);

  // Phương thức từ Player
  void addMove(Player::Move& move, Board& board);
  std::vector<Player::Move> getMoves() const;
  Player::Move SwapTiles(Bag& bag);
  static Player::Move PassMove();
  Player::Move FindBestMove(Board& board, Lexicon& lexicon, Bag& bag);

  // Các phương thức hỗ trợ cho logic AI
  std::vector<Player::Move> GenerateAllMoves(Board& board, Lexicon& lexicon);
  static bool IsAnchor(int row, int col, Board& board);
  void GenerateMoveFromAnchor(bool horizontal, Board& board, Lexicon& lexicon,
                              std::vector<Player::Move>& moves);
  bool IsValidMove(const Player::Move& move, Board& board);
  void GenerateWords(Node* node, MoveGenState& state, Board& board,
                     Lexicon& lexicon, std::vector<Player::Move>& moves,
                     int row, int col);
  void ProcessOccupiedCell(Node* node, MoveGenState& state, Board& board,
                           Lexicon& lexicon, std::vector<Player::Move>& moves,
                           int row, int col);
  void ProcessEmptyCell(Node* node, MoveGenState& state, Board& board,
                        Lexicon& lexicon, std::vector<Player::Move>& moves,
                        int& row, int& col);
  void NormalTileCase(Node* node, MoveGenState& state, Board& board,
                      Lexicon& lexicon, std::vector<Player::Move>& moves,
                      int& row, int& col);
  void BlankTileCase(Node* node, MoveGenState& state, Board& board,
                     Lexicon& lexicon, std::vector<Player::Move>& moves,
                     int& row, int& col);
  void ProcessTile(Node* node, MoveGenState& state, Board& board,
                   Lexicon& lexicon, std::vector<Player::Move>& moves, int row,
                   int col, char c, int tile_index);
  void AutoPlaceTile(Board& board, Lexicon& lexicon, Bag& bag);

 private:
  std::vector<Player::Move> moves_;  // Kế thừa từ Player
  std::unique_ptr<Lexicon>
      lexicon_;  // Lưu trữ con trỏ tới lexicon để sử dụng trong IsValidMove
  void BuildCrossWordSegment(Board& board, const Board::Move& move, int& r,
                             int& c, bool cross_horizontal, bool forward,
                             std::string& word);
  char FindMoveLetter(const Board::Move& move, int row, int col,
                      bool& found) const;
};
};  // namespace core