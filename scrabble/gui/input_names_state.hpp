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
  struct InputBox {
    SDL_Rect box;
    std::string text;
  };

  InputNamesState(GUI* gui, int num_inputs);

  std::vector<std::string> GetInputs() const;
  
  void Render(SDL_Renderer* renderer) override;
  void HandleEvent(SDL_Event& event) override;
  void Update() override;

 private:
  GUI* gui_;

  std::vector<InputBox> input_boxes_;
  int focused_box_ = 0;
  
  SDL_Rect back_button_{};
  SDL_Rect create_game_button_{};

  std::string error_message;
  bool ValidateInputs();
  
  void RenderInputBoxes(SDL_Renderer* renderer, TTF_Font* font);
  void HandleKeystrokes(SDL_Event& event, std::string& input);
};
}  // namespace gui
