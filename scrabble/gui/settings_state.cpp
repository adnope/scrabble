#include "settings_state.hpp"

#include <string>

#include "SDL_mouse.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "core/dictionary.hpp"
#include "gui/gui.hpp"

namespace gui {
void SettingsState::HandleArrows(SDL_Event& event) {
  for (Option& option : option_list_) {
    SDL_Point mouse_pos = {event.motion.x, event.motion.y};
    if (SDL_PointInRect(&mouse_pos, &option.leftarrow_rect) == 1) {
      option.current_option_index = static_cast<int>(
          (option.current_option_index - 1) % option.options.size());
    }
    if (SDL_PointInRect(&mouse_pos, &option.rightarrow_rect) == 1) {
      option.current_option_index = static_cast<int>(
          (option.current_option_index + 1) % option.options.size());
    }
    if (option.label == "Resolution") {
      if (option.current_option() == "1280x720" &&
          gui_->current_resolution() != "1280x720") {
        gui_->SetSize(1280, 720);
      }
      if (option.current_option() == "1600x900" &&
          gui_->current_resolution() != "1600x900") {
        gui_->SetSize(1600, 900);
      }
      if (option.current_option() == "1920x1080" &&
          gui_->current_resolution() != "1920x1080") {
        gui_->SetSize(1920, 1080);
      }
    }
  }
}

void SettingsState::HandleHovering(SDL_Event& event) {
  SDL_Point mouse_pos{event.motion.x, event.motion.y};

  bool hovering_arrow = false;
  for (const Option& option : option_list_) {
    mouse_pos = {event.motion.x, event.motion.y};
    hovering_arrow = SDL_PointInRect(&mouse_pos, &option.leftarrow_rect) == 1 ||
                     SDL_PointInRect(&mouse_pos, &option.rightarrow_rect) == 1;
    if (hovering_arrow) {
      break;
    }
  }

  is_hovering_ =
      SDL_PointInRect(&mouse_pos, &back_button_) == 1 || hovering_arrow;

  if (is_hovering_) {
    SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_HAND));
  } else {
    SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_ARROW));
  }
}

void SettingsState::HandleEvent(SDL_Event& event) {
  HandleHovering(event);
  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    SDL_Point mouse_pos{event.motion.x, event.motion.y};
    if (SDL_PointInRect(&mouse_pos, &back_button_) == 1) {
      gui_->ChangeState(GUI::GameStateType::MainMenu);
      return;
    }
    HandleArrows(event);
  }
}

void SettingsState::Update() {
  for (const Option& option : option_list_) {
    if (option.label == "VSync") {
      if (option.current_option() == "On" && !gui_->vsync()) {
        gui_->SetVsync(true);
      }
      if (option.current_option() == "Off" && gui_->vsync()) {
        gui_->SetVsync(false);
      }
    }
    if (option.label == "Dictionary") {
      if (option.current_option() == "TWL" &&
          gui_->lexicon_type() != core::Dictionary::TWL) {
        gui_->SetLexiconType(core::Dictionary::TWL);
      }
      if (option.current_option() == "CSW" &&
          gui_->lexicon_type() != core::Dictionary::CSW) {
        gui_->SetLexiconType(core::Dictionary::CSW);
      }
    }
  }
}

void SettingsState::Render(SDL_Renderer* renderer) {
  gui_->RenderImage(renderer,
                    "assets/textures/settings/settings_background.png",
                    {0, 0, gui_->window_width(), gui_->window_height()});
  const int w_width = gui_->window_width();
  const int w_height = gui_->window_height();
  const int num_options = static_cast<int>(option_list_.size());

  const int box_w = w_width * 5 / 8;
  const int box_h = w_height * 5 / 8;
  const int box_x = (w_width / 2) - (box_w / 2);
  const int box_y = w_height * 2 / 9;
  settings_box_ = {box_x, box_y, box_w, box_h};
  gui_->RenderImage(renderer, "assets/textures/settings/settings_box.png",
                    settings_box_);

  back_button_.w = w_width * 5 / 32;
  back_button_.h = w_height / 10;
  back_button_.x = (w_width / 2) - (back_button_.w / 2);
  back_button_.y = settings_box_.y + settings_box_.h + w_height / 24;
  gui_->RenderImage(renderer, "assets/textures/settings/button_back.png",
                    back_button_);
  const int padding_x = box_w / 20;
  const int padding_y = box_h / 20;
  const int option_h = box_h / 10;
  const int gap = box_h / 30;
  const int initial_y = box_y + padding_y;

  for (int i = 0; i < num_options; ++i) {
    const int label_x = box_x + padding_x;
    const int label_y = initial_y + (i * (option_h + gap));
    const int current_option_x = box_x + box_w - (box_w / 4);
    GUI::RenderFixedHeightText(renderer, option_list_[i].label,
                               gui_->jersey32(), label_x, label_y, option_h,
                               {255, 255, 255, 255});
    GUI::RenderFixedHeightCenteredText(
        renderer, option_list_[i].current_option(), gui_->jersey32(),
        current_option_x, label_y, option_h, {255, 255, 255, 255},
        option_list_[i].current_option_rect.w);
    const int arrow_y = label_y;
    const int arrow_w = box_w / 40;
    const int arrow_h = option_h;
    const int leftarrow_x = current_option_x -
                            (option_list_[i].current_option_rect.w / 2) -
                            (option_list_[i].current_option_rect.w / 4);
    option_list_[i].leftarrow_rect = {leftarrow_x, arrow_y, arrow_w, arrow_h};
    gui_->RenderImage(renderer, "assets/textures/settings/left_arrow.png",
                      option_list_[i].leftarrow_rect);
    const int rightarrow_x = current_option_x +
                             (option_list_[i].current_option_rect.w / 2) +
                             (option_list_[i].current_option_rect.w / 44);
    option_list_[i].rightarrow_rect = {rightarrow_x, arrow_y, arrow_w, arrow_h};
    gui_->RenderImage(renderer, "assets/textures/settings/right_arrow.png",
                      option_list_[i].rightarrow_rect);
  }
}
}  // namespace gui
