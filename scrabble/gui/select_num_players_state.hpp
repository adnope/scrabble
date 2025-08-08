#pragma once

#include "SDL_rect.h"
#include "gui.hpp"
#include "i_game_state.hpp"

namespace gui {
class SelectNumPlayersState : public IGameState {
 public:
  explicit SelectNumPlayersState(GUI* gui) : gui_(gui) {}

  void HandleEvent(SDL_Event& event) override;

  void Update() override;

  void Render(SDL_Renderer* renderer) override;

 private:
  GUI* gui_;
  SDL_Rect two_players_button_{};
  SDL_Rect three_players_button_{};
  SDL_Rect four_players_button_{};
  SDL_Rect back_button_{};
};
}  // namespace gui
