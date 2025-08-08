#pragma once

#include <string>
#include <vector>

#include "game/game.hpp"

namespace cli {
class CLIGame {
 public:
  using PlayerAction = game::Game::MoveType;

  CLIGame() {}

  void Start() {
    InitDictionary();
    InitPlayers();
    InitFirstPlayer();
    InitPlayerDecks();

    StartGameLoop();
    End();
  }

  int NumPlayers() const { return game.num_players(); }

 private:
  game::Game game;

  static int RequestNumPlayers();

  static std::vector<std::string> RequestPlayerNames(int num_players);

  void PrintSwappingIndices(const std::vector<int>& indices);

  void PrintMoveHistory();

  PlayerAction RequestPlayerAction();

  bool ValidateSwappingIndicesInput(const std::string& input,
                                    std::vector<int>& indices);

  std::vector<int> RequestSwappingIndices();

  static void InitDictionary();

  void InitPlayers();

  void InitFirstPlayer();

  void InitPlayerDecks() { game.InitPlayerDecks(); }

  void StartGameLoop();

  bool ExecutePassMove();

  bool ExecuteSwapMove();

  void End();
};
}  // namespace cli
