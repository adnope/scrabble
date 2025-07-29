#include "bot.hpp"

#include <algorithm>
#include <vector>

#include "core/board.hpp"
#include "core/lexicon.hpp"
#include "core/tile.hpp"
#include "word.hpp"

namespace core {

void Bot::addMove(Board::Move &move) { moves.push_back(move); }
std::vector<Board::Move> Bot::getMoves() const { return moves; }

Board::Move Bot::FindBestMove(const std::vector<Tile>& rack, Board& board,
                              Lexicon& lexicon) {
  std::vector<Board::Move> allMoves = GenerateAllMoves(rack, board, lexicon);
  // Board::Move bestMove;
  // Init best_Move, maxScore
  auto best_Move = allMoves[0];
  auto maxScore = board.ValidateMove(best_Move, lexicon).move_points;
  // int maxScore = -1;
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
                                               Board& board,
                                               Lexicon& lexicon) {
  std::vector<Board::Move> allMoves;

  for (int row = 0; row < Board::kHeight; row++) {
    for (int col = 0; col < Board::kWidth; col++) {
      if (IsAnchor(row, col, board)) {
        // Generate all possible moves for this anchor
        Board::Move move;
        GenerateMoveFromAnchor(row, col, true, rack, board, lexicon, move,
                               allMoves);
        move.clear();  // Đặt lại move cho hướng dọc
        GenerateMoveFromAnchor(row, col, false, rack, board, lexicon, move,
                               allMoves);
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

void Bot::GenerateMoveFromAnchor(int row, int col, bool horizontal,
                                 const std::vector<Tile>& rack, Board& board,
                                 Lexicon& lexicon, Board::Move& move,
                                 std::vector<Board::Move>& moves) {
  std::map<char, int> freq;
  for (const auto& tile : rack) {
    freq[tile.letter()]++;
  }
  std::vector<Tile> rackCopy(rack.begin(), rack.end());
  MoveGenState state(std::move(freq), std::string(), horizontal,
                     false, std::move(rackCopy));
  int case_curr = (row * Board::kWidth) + col;
  GenerateWords(lexicon.getRoot(), state, case_curr, board, lexicon, move,moves);
  if (!moves.empty()) {
    moves.push_back(move);
  }
}

bool Bot::VerifyCrossWords(int case_curr, char c, Board& board,
                           Lexicon& lexicon, const Board::Move& move) {
  int row = case_curr / Board::kWidth;
  int col = case_curr % Board::kWidth;

  std::string horizontalWord =
      board.GetWordFromPos(row, col, true, move).AsString();
  if (!horizontalWord.empty()) {
    horizontalWord += c;
    if (!lexicon.Contains(horizontalWord)) {
      return false;
    }
  }

  std::string verticalWord =
      board.GetWordFromPos(row, col, false, move).AsString();
  if (!verticalWord.empty()) {
    verticalWord += c;
    if (!lexicon.Contains(verticalWord)) {
      return false;
    }
  }

  return true;
}

void Bot::GenerateWords(Node* node, MoveGenState &state, int case_curr, Board& board, Lexicon& lexicon, Board::Move& currentMove, std::vector<Board::Move> moves) {
    int row = case_curr / Board::kWidth;
    int col = case_curr % Board::kWidth;

    if (row < 0 || row >= Board::kHeight || col < 0 || col >= Board::kWidth) {
        return;
    }

    if (node->is_word && !state.mot.empty() && !state.plus && currentMove.size() > 0) {
        auto [words, score, status] = board.ValidateMove(currentMove, lexicon);
        if (status == Board::ResponseStatus::kSuccess) {
            moves.push_back(currentMove);
        }
    }

    if (board.IsOccupied(row, col)) {
        ProcessOccupiedCell(node, state, case_curr, board, lexicon, currentMove,moves);
    } else {
        ProcessEmptyCell(node, state, case_curr, board, lexicon, currentMove, moves);
    }
}

void Bot::ProcessOccupiedCell(Node* node, MoveGenState& state, int case_curr, Board& board, Lexicon& lexicon, Board::Move& currentMove, std::vector<Board::Move>& moves) {
    Tile tile = board.GetTile(case_curr);
    char c = tile.letter();
    auto it = node->suffixes.find(c);
    if (it != node->suffixes.end()) {
        std::map<char, int> new_freq = state.freq;
        std::string new_mot = state.mot + c;
        std::vector<Tile> new_rack = state.rack;
        MoveGenState new_state(std::move(new_freq), std::move(new_mot), state.orientation, state.plus, std::move(new_rack));
        int next_case = state.orientation ? case_curr + Board::kWidth : case_curr + 1;
        GenerateNextMove(it->second.get(), new_state, next_case, board, lexicon, currentMove, moves);
    }
}

void Bot::ProcessEmptyCell(Node* node, MoveGenState& state, int case_curr, Board& board, Lexicon& lexicon, Board::Move& currentMove, std::vector<Board::Move>& moves) {
    std::vector<char> unique_letters;
    for (const auto& tile : state.rack) {
        char c = tile.letter();
        if (std::find(unique_letters.begin(), unique_letters.end(), c) == unique_letters.end()) {
            unique_letters.push_back(c);
        }
    }
    if (!state.plus) {
        unique_letters.push_back('+');
    }

    for (char c : unique_letters) {
        auto it = node->suffixes.find(c);
        if (it == node->suffixes.end()) {
            continue;
        }

        if (c != '+' && !VerifyCrossWords(case_curr, c, board, lexicon, currentMove)) {
            continue;
        }

        std::map<char, int> new_freq = state.freq;
        std::string new_mot = state.mot + c;
        std::vector<Tile> new_rack = state.rack;
        Board::Move new_move = currentMove;
        bool is_plus = (c == '+');

        if (!is_plus) {
            if (new_freq[c] <= 0) {
                continue;
            }
            new_freq[c]--;
            auto tile_it = std::find_if(new_rack.begin(), new_rack.end(),
                [c](const Tile& t) { return t.letter() == c; });
            if (tile_it != new_rack.end()) {
                new_move.emplace_back(*tile_it, case_curr / Board::kWidth, case_curr % Board::kWidth);
                new_rack.erase(tile_it);
            }
        }

        MoveGenState new_state(std::move(new_freq), std::move(new_mot), state.orientation, is_plus ? true : state.plus, std::move(new_rack));
        int next_case = state.orientation ? case_curr + Board::kWidth : case_curr + 1;
        GenerateNextMove(it->second.get(), new_state, next_case, board, lexicon, new_move, moves);
    }
}

void Bot::GenerateNextMove(Node* node, MoveGenState& state, int next_case, Board& board, Lexicon& lexicon, Board::Move& currentMove, std::vector<Board::Move>& moves) {
    GenerateWords(node, state, next_case, board, lexicon, currentMove, moves);
}

};  // namespace core
