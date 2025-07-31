#pragma once

#include "game_state.hpp"
#include "gui.hpp"

namespace gui {
class CreateNewGameState : public IGameState {
 public:
  explicit CreateNewGameState(GUI* gui) : gui_(gui) {}

  void HandleEvent(SDL_Event& event) override;

  void Update() override;

  void Render(SDL_Renderer* renderer) override;

 private:
  GUI* gui_;

  void RenderImage(SDL_Renderer* renderer, const std::string& image_path,
                   SDL_Rect area);
};
}  // namespace gui
