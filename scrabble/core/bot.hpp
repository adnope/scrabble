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

  //Sinh từ từ điểm neo
  // Cần hàm lấy giá tọa độ trong Board lấy từng tile cộng vào tile đang có
  //Gọi ProcessOccupiedCell và ProcessEmptyCell trong các trường hợp tương ứng
  static void GenerateMoveFromAnchor(bool horizontal,
                                     const std::vector<Tile>& rack,
                                     Board& board, Lexicon& lexicon, 
                                     std::vector<Board::Move>& moves);
  bool IsValidMove(const Board::Move& move, Board& board) const;
  static bool VerifyCrossWords(char c, Board& board,
                               Lexicon& lexicon, const Board::Move& move);
  static void GenerateWords(Node* node, MoveGenState& state,
                            Board& board, Lexicon& lexicon,
                            std::vector<Board::Move>& moves);
  std::vector<Board::Move> moves;
  //ProcessOccupiedCell
  //ProcessEmptyCell
  //Gọi cả 2 hàm xử lý NormalTileCase (không blank tile trên tay), BlankTileCase(có 1-2 blank tile trên tay)


  //1 dãy có nghĩa, nối thêm vào dãy này
  // sử dụng GetWordFromPos lấy từ -> truyền cho tileData trước khi gọi các hàm con
  static void ProcessOccupiedCell(Node* node, MoveGenState& state,
                                  Board& board, Lexicon& lexicon,
                                  std::vector<Board::Move>& moves);

  //Điền tự do, có điểm neo
  static void ProcessEmptyCell(Node* node, MoveGenState& state,
                               Board& board, Lexicon& lexicon,
                               std::vector<Board::Move>& moves);

  // Sinh từ từ tilesData(từ có nghĩa sẵn) + state.rack =  tile trên tay
  // vd data = sign; rack =  a m e n t -> asignment 
  // tuong tu rack = a ? e n t
  // tileData rỗng -> ProcessEmptyCell gọi tới 
  // tileData có du lieu -> ProcessOccupiedCell goi toi

  // No blank tile
  static void NormalTileCase(Node* node, const std::vector<Tile>& tilesData,
                             MoveGenState& state, Lexicon& lexicon,
                             std::vector<Board::Move>& moves);


  // With blank tile 
  // đẩy các tile '?' xuống cuối cùng , sử dụng NormalTileCase() cho các chữ thật ->sau đó truy vấn node cho các '?'
  // -:> giảm truy vấn tree
  static void BlankTileCase(Node* node, const std::vector<Tile>& tilesData,
                            MoveGenState& state, Lexicon& lexicon,
                            std::vector<Board::Move>& moves);

 // Swap -> cập nhật MoveGenstate
 // Điều kiện Swap -> không có bước đi bestMove = rỗng
 // Swap có lợi hơn pass -> bot spam swáp ;))
};

};  // namespace core
