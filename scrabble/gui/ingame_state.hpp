#pragma once

#include <array>
#include <string>
#include <vector>

#include "SDL_events.h"
#include "SDL_render.h"
#include "core/dictionary.hpp"
#include "game/game.hpp"
#include "gui.hpp"
#include "i_game_state.hpp"

namespace gui {
class IngameState : public IGameState {
 public:
  static constexpr int kBoardSize = 15;

  struct PlayerInfoBox {
    SDL_Rect box;
    std::string name;
    int score;
  };

  struct HistoryEntry {
    SDL_Rect box;
    std::string player_name;
    std::string move_type;
    std::string move_content;
  };

  IngameState(GUI* gui, core::Dictionary::DictionaryType dict_type,
              const std::vector<std::string>& player_names);

  void HandleEvent(SDL_Event& event) override;

  void Update() override;

  void Render(SDL_Renderer* renderer) override;

 private:
  GUI* gui_;
  game::Game game_;

  std::array<std::array<SDL_Rect, kBoardSize>, kBoardSize> board_grid_{};
  std::array<SDL_Rect, 7> deck_{};
  std::vector<PlayerInfoBox> player_infos_;
  std::vector<HistoryEntry> movehistory_;

  SDL_Rect submit_button_{};
  SDL_Rect swap_button_{};
  SDL_Rect pass_button_{};

  SDL_Rect movehistory_box_{};

  void InitFirstPlayer();
  void InitPlayerDecks();

  void UpdateBoardSize();
  void RenderBoard(SDL_Renderer* renderer);

  void UpdateDeckSize();
  void RenderDeck(SDL_Renderer* renderer);

  void UpdatePlayerinfoBoxesSize();
  void RenderPlayerinfoBoxes(SDL_Renderer* renderer);

  void UpdateActionButtonsSize();
  void RenderActionButtons(SDL_Renderer* renderer);

  void UpdateMoveHistorySize();
  void RenderMoveHistory(SDL_Renderer* renderer);

  void HandleTileDrag(SDL_Event& event);
};
}  // namespace gui
