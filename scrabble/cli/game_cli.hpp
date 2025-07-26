#pragma once

#include "core/dictionary.hpp"
#include "game/game.hpp"

namespace cli {
class CLIGame {
 public:
  using PlayerAction = game::Game::MoveType;

  CLIGame() : game(core::Dictionary::TWL) {}

  void Start() {
    InitDictionary();
    InitPlayers();
    InitFirstPlayer();
    InitPlayerDecks();
    game.PrintDebugInfo();

    StartGameLoop();
    End();
  }

  int NumPlayers() const { return game.NumPlayers(); }

 private:
  game::Game game;

  static int RequestNumPlayers();

  static std::vector<std::string> RequestPlayerNames(int num_players);

  PlayerAction RequestPlayerAction();

  std::vector<int> RequestSwappingIndices();

  void InitDictionary();

  void InitPlayers();

  void InitFirstPlayer();

  void InitPlayerDecks() { game.InitPlayerDecks(); }

  void StartGameLoop();

  void End();
};
}  // namespace cli