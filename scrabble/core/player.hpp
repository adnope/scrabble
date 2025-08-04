#pragma once

#include <array>
#include <string>
#include <vector>

#include "bag.hpp"
#include "board.hpp"
#include "tile.hpp"

namespace core {
class Player {
 public:
  struct Placement {
    int tile_index = 0;
    int row = 0;
    int col = 0;
  };

  struct MoveSubmissionResponse {
    std::vector<Word> words;
    int move_points;
    Board::ResponseStatus status;

    explicit MoveSubmissionResponse(
        const Board::MoveValidationResponse &board_response)
        : words(board_response.words),
          move_points(board_response.move_points),
          status(board_response.status) {}
  };

  using Move = std::vector<Placement>;

  static constexpr int kMaxDeckSize = 7;

  Player(const std::string &player_name, int score);

  void AddTile(const Tile &tile);

  void AddTiles(const std::vector<Tile> &tiles);

  void DrawNewTiles(Bag &bag);

  void UseTile(int index);

  void AddScore(const int score) { player_score_ += score; }

  void SubtractScore(const int score) { player_score_ -= score; }

  int GetDeckScore() const;

  bool PerformSwap(Bag &bag, const std::vector<int> &indices);

  MoveSubmissionResponse SubmitMove(const Move &move, Board &board,
                                    const Lexicon &lexicon);

  void PrintDeck() const;

  std::string name() const { return player_name_; }
  int score() const { return player_score_; }
  std::array<Tile, kMaxDeckSize> deck() { return deck_; }
  int current_deck_size() const;
  void SwapTile(int index, Bag &bag);
  static Board::Move ConvertToBoardMove(Player& player, const Player::Move &player_move);

 private:
  std::string player_name_;
  std::array<Tile, kMaxDeckSize> deck_;
  int player_score_;

};
}  // namespace core
