#include "input_names_state.hpp"

#include <SDL_render.h>
#include <SDL_surface.h>
#include <SDL_ttf.h>

#include <cctype>
#include <string>

#include "SDL_events.h"
#include "SDL_mouse.h"
#include "SDL_rect.h"
#include "gui/gui.hpp"

namespace gui {

InputNamesState::InputNamesState(GUI* gui, int num_inputs)
    : gui_(gui), input_boxes_(num_inputs) {}

std::vector<std::string> InputNamesState::GetInputs() const {
  std::vector<std::string> inputs(input_boxes_.size());
  for (int i = 0; i < static_cast<int>(input_boxes_.size()); ++i) {
    inputs[i] = input_boxes_[i].text;
  }
  return inputs;
}

bool InputNamesState::ValidateInputs() {
  for (int i = 0; i < static_cast<int>(input_boxes_.size()); ++i) {
    auto& box = input_boxes_[i];
    auto name = box.text;

    if (name.empty()) {
      error_message =
          "You didn't enter player " + std::to_string(i + 1) + "'s name.";
      return false;
    }
    if (name.size() < 3) {
      error_message = "Invalid name: " + name +
                      ". Player name must be longer than 3 characters.";
      return false;
    }
    for (const char c : name) {
      if (std::isalnum(static_cast<unsigned char>(c)) == 0) {
        error_message =
            "Invalid name: " + name +
            ". Player name must contain alphabet letters and digits only.";
        return false;
      }
    }
    for (int j = i + 1; j < static_cast<int>(input_boxes_.size()); ++j) {
      if (name == input_boxes_[j].text) {
        error_message = "Player " + std::to_string(i + 1) + " and Player " +
                        std::to_string(j + 1) + " have the same name.";
        return false;
      }
    }
  }

  return true;
}

void InputNamesState::RenderInputBoxes(SDL_Renderer* renderer, TTF_Font* font) {
  const int box_w = gui_->window_width() * 2 / 5;
  const int box_h = gui_->window_height() * 1 / 9;
  const int box_x = (gui_->window_width() - box_w) / 2;
  const int box_initial_y = gui_->window_height() * 11 / 40;
  const int box_gap = gui_->window_height() * 1 / 30;

  for (int i = 0; i < static_cast<int>(input_boxes_.size()); ++i) {
    auto& box = input_boxes_[i].box;
    box.w = box_w;
    box.h = box_h;
    box.x = box_x;
    box.y = box_initial_y + i * (box_gap + box_h);

    if (focused_box_ == i) {
      gui_->RenderImage(
          renderer, "assets/textures/input_names/focused_box_background.png",
          {box.x - 4, box.y - 4, box.w + 8, box.h + 8});
    }
    std::string image_path = "assets/textures/input_names/";
    image_path += "input_box_" + std::to_string(i + 1) + ".png";
    gui_->RenderImage(renderer, image_path, box);

    const int text_padding_x = box.w / 40;
    const int text_x = box.x + text_padding_x;
    const int text_y = box.y + (box.h / 3);
    constexpr SDL_Color kBlack = {0, 0, 0, 255};
    GUI::RenderText(renderer, input_boxes_[i].text, font, text_x, text_y,
                    kBlack);
  }
}

void InputNamesState::Render(SDL_Renderer* renderer) {
  const int w_width = gui_->window_width();
  const int w_height = gui_->window_height();

  gui_->RenderImage(renderer,
                    "assets/textures/input_names/input_names_background.png",
                    {0, 0, w_width, w_height});
  RenderInputBoxes(renderer, gui_->jersey48());

  create_game_button_.w = w_width * 1 / 4;
  create_game_button_.h = w_height * 1 / 12;
  create_game_button_.x = (w_width - create_game_button_.w) / 2;
  create_game_button_.y = w_height * 8 / 9;
  gui_->RenderImage(renderer,
                    "assets/textures/input_names/button_create_new_game.png",
                    create_game_button_);

  back_button_.w = w_height / 12;
  back_button_.h = w_height / 12;
  back_button_.x = w_width / 50;
  back_button_.y = w_width / 50;

  gui_->RenderImage(renderer, "assets/textures/input_names/home.png",
                    back_button_);

  if (!error_message.empty()) {
    GUI::RenderTextCenteredX(
        renderer, gui_->jersey32(), error_message, w_width / 2,
        create_game_button_.y - (w_height / 18), {255, 0, 0, 255});
  }
}

void InputNamesState::HandleKeystrokes(SDL_Event& event, std::string& input) {
  if (event.type == SDL_TEXTINPUT) {
    if (input.size() < 14) {
      input += static_cast<const char*>(event.text.text);
    }
  }

  if (event.type == SDL_KEYDOWN) {
    switch (event.key.keysym.sym) {
      case SDLK_BACKSPACE:
        if (!input.empty()) {
          input.pop_back();
        }
        break;

      case SDLK_RETURN:
      case SDLK_KP_ENTER:
        if (ValidateInputs()) {
          gui_->ChangeState(GUI::GameStateType::Ingame, GetInputs());
        }
        break;
      default:
        break;
    }
  }
}

void InputNamesState::HandleEvent(SDL_Event& event) {
  HandleKeystrokes(event, input_boxes_[focused_box_].text);

  SDL_Point mouse_pos{event.motion.x, event.motion.y};
  bool is_hovering = false;
  for (const auto& box : input_boxes_) {
    if (SDL_PointInRect(&mouse_pos, &box.box) == 1) {
      is_hovering = true;
    }
  }
  if (SDL_PointInRect(&mouse_pos, &create_game_button_) == 1 ||
      SDL_PointInRect(&mouse_pos, &back_button_) == 1) {
    is_hovering = true;
  }

  if (is_hovering) {
    SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_HAND));
  } else {
    SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_ARROW));
  }

  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    for (int i = 0; i < static_cast<int>(input_boxes_.size()); ++i) {
      auto& box = input_boxes_[i];
      if (SDL_PointInRect(&mouse_pos, &box.box) == 1) {
        focused_box_ = i;
        error_message.clear();
      }
    }
    if (SDL_PointInRect(&mouse_pos, &create_game_button_) == 1 &&
        ValidateInputs()) {
      gui_->ChangeState(GUI::GameStateType::Ingame, GetInputs());
    }
    if (SDL_PointInRect(&mouse_pos, &back_button_) == 1) {
      gui_->ChangeState(GUI::GameStateType::SelectNumPlayers);
    }
  }
}

void InputNamesState::Update() {}
}  // namespace gui
