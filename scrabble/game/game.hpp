#pragma once

#include "bag.hpp"
#include "board.hpp"
#include "core/dictionary.hpp"
#include "player.hpp"

namespace core {

struct Move {
  std::string player_name;
  std::vector<std::string> words;
  int row;
  int col;
  bool horizontal;
  int score;
  std::vector<int> tile_indices;
  bool is_pass;
  std::string move_type;
};

class Game {
 public:
  Game(int num_players, Dictionary::DictionaryType dict_type,
       const std::vector<std::string>& custom_names = {});
  bool IsGameOver() const;
  void NextTurn();
  bool ExecuteMove(const std::string& move_type, const std::string& word,
                   char dir, int row, int col);
  Player GetWinner() const;
  const Player& GetCurrentPlayer() const;
  int GetBagSize() const { return bag_.num_tiles_remanining(); }
  void EndGame();
  const std::vector<Move>& GetMoveHistory() const {
    return move_history_;
  }

 private:
 //Xử lý các lượt -> lưu thông tin các hành động
 //Các hàm này đc implement để có thể dễ dàng gọi lại trong ExecuteMove vốn khá phức tạp bởi 
 //trong game có rất nhiều hành động diễn ra
  void ExecutePassMove();
  bool ExecutePlaceMove(const std::string& word, char dir, int row, int col,
                        Move& move);
  bool ExecuteSwapMove(const std::string& word, Move& move);
  bool GetTileIndices(const std::string& word, std::vector<int>& tile_indices);

  std::vector<Player> players_;
  Bag bag_;
  Board board_;
  Dictionary dictionary_;
  int current_player_index_;
  int consecutive_passes_;
  bool game_over_;
  std::vector<Move> move_history_;
};
}  // namespace core