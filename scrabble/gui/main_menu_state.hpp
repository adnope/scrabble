#pragma once

#include "SDL_events.h"
#include "SDL_rect.h"
#include "gui.hpp"
#include "i_game_state.hpp"

namespace gui {
class MainMenuState : public IGameState {
 public:
  explicit MainMenuState(GUI* gui) : gui_(gui) {}

  void HandleEvent(SDL_Event& event) override;

  void Update() override;

  void Render(SDL_Renderer* renderer) override;

 private:
  GUI* gui_;

  SDL_Rect newgame_button_{};
  SDL_Rect settings_button_{};
  SDL_Rect quit_button_{};
};
}  // namespace gui
