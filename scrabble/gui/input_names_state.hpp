#pragma once

#include <string>
#include <vector>

#include "SDL_events.h"
#include "SDL_rect.h"
#include "gui.hpp"
#include "i_game_state.hpp"

namespace gui {
class InputNamesState : public IGameState {
 public:
  InputNamesState(GUI* gui, int num_inputs);

  void HandleEvent(SDL_Event& event) override;
  void Update() override;
  void Render(SDL_Renderer* renderer) override;

  const std::vector<std::string>& GetInputs() const { return inputs_; }

 private:
  GUI* gui_;
  int focused_box_ = -1;

  std::vector<SDL_Rect> input_boxes_;
  std::vector<std::string> inputs_;
  SDL_Rect start_button_{};
};
}  // namespace gui
