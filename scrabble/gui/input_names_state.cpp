#include "input_names_state.hpp"

#include <SDL_render.h>
#include <SDL_surface.h>
#include <SDL_ttf.h>

namespace gui {

InputNamesState::InputNamesState(GUI* gui, int num_inputs)
    : gui_(gui), input_boxes_(num_inputs), inputs_(num_inputs) {
  const int w = gui_->window_width();
  const int h = gui_->window_height();
  const int box_width = w / 2;
  const int box_height = h / 10;
  const int x = (w - box_width) / 2;
  const int gap = box_height + (h / 30);
  const int start_y = h / 4;

  for (int i = 0; i < num_inputs; ++i) {
    input_boxes_[i] = {x, start_y + (i * gap), box_width, box_height};
  }

  start_button_ = {x, start_y + (num_inputs * gap), box_width, box_height};
}

void InputNamesState::Update() {}

void InputNamesState::HandleEvent(SDL_Event& event) {
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    SDL_Point mouse{event.button.x, event.button.y};
    focused_box_ = -1;
    for (int i = 0; i < static_cast<int>(input_boxes_.size()); ++i) {
      if (SDL_PointInRect(&mouse, &input_boxes_[i]) == 1) {
        focused_box_ = i;
      }
    }
    if (SDL_PointInRect(&mouse, &start_button_) == 1) {
      gui_->ChangeState(
          GUI::GameStateType::EndGame);  // Replace or store inputs if needed
    }
  } else if (event.type == SDL_TEXTINPUT && focused_box_ != -1) {
    inputs_[focused_box_] += event.text.text;
  } else if (event.type == SDL_KEYDOWN && focused_box_ != -1) {
    if (event.key.keysym.sym == SDLK_BACKSPACE &&
        !inputs_[focused_box_].empty()) {
      inputs_[focused_box_].pop_back();
    }
  }
}

void InputNamesState::Render(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
  SDL_RenderClear(renderer);

  const double w_width = gui_->window_width();
  const double w_height = gui_->window_height();

  GUI::RenderText(renderer, "Input players name", gui_->jersey32(), w_width / 2,
                  w_height / 30, {255, 255, 255, 255});

  gui_->RenderImage(
      renderer, "assets/textures/blank_background.png",
      {0, 0, static_cast<int>(w_width), static_cast<int>(w_height)});

  for (int i = 0; i < static_cast<int>(input_boxes_.size()); ++i) {
    gui_->RenderImage(renderer, "assets/textures/blank_field.png",
                     input_boxes_[i]);
  }

  gui_->RenderImage(renderer, "assets/textures/button_startgame.png",
                   start_button_);
}
}  // namespace gui
