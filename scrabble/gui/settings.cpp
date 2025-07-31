#include "settings.hpp"

#include <string>

#include "SDL_image.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_ttf.h"

namespace gui {
void SettingsState::HandleArrows(SDL_Event& event) {
  SDL_Point mouse_pos;
  if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN) {
    mouse_pos.x = event.motion.x;
    mouse_pos.y = event.motion.y;

    for (Option& option : option_list_) {
      bool is_hovering_right =
          SDL_PointInRect(&mouse_pos, &option.rightarrow_area);
      bool is_hovering_left =
          SDL_PointInRect(&mouse_pos, &option.leftarrow_area);
      if (is_hovering_left || is_hovering_right) {
        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
        if (event.type == SDL_MOUSEBUTTONDOWN &&
            event.button.button == SDL_BUTTON_LEFT) {
          if (is_hovering_left) {
            option.current_option_index =
                (option.current_option_index - 1) % option.options.size();
          }
          if (is_hovering_right) {
            option.current_option_index =
                (option.current_option_index + 1) % option.options.size();
          }
          if (option.label == "Resolution") {
            if (option.current_option() == "1920x1080") {
              gui_->SetSize(1920, 1080);
            }
            if (option.current_option() == "1280x720") {
              gui_->SetSize(1280, 720);
            }
          }
          if (option.label == "VSync") {
            gui_->ToggleVSync();
          }
        }
      } else {
        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
      }
    }
  }
}

void SettingsState::HandleEvent(SDL_Event& event) {
  SDL_Point mouse_pos;
  if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN) {
    mouse_pos.x = event.motion.x;
    mouse_pos.y = event.motion.y;

    bool is_hovering_back = SDL_PointInRect(&mouse_pos, &back_button_);

    if (is_hovering_back) {
      SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
      if (event.type == SDL_MOUSEBUTTONDOWN &&
          event.button.button == SDL_BUTTON_LEFT) {
        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
        gui_->ChangeState(GUI::GameStateType::MainMenu);
      }
    } else {
      SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
      HandleArrows(event);
    }
  }
}

void SettingsState::Update() {
  // e.g. animate background
}

void SettingsState::RenderImage(SDL_Renderer* renderer,
                                const std::string& image_path, SDL_Rect area) {
  SDL_Surface* surface = IMG_Load(image_path.c_str());
  if (surface == nullptr) {
    SDL_Log("IMG_Load failed for %s: %s", image_path.c_str(), IMG_GetError());
    return;
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);

  if (texture == nullptr) {
    SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
    return;
  }

  SDL_RenderCopy(renderer, texture, nullptr, &area);
  SDL_DestroyTexture(texture);
}

void SettingsState::Render(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  RenderImage(renderer, "assets/textures/settings_background_2.png",
              {0, 0, gui_->window_width(), gui_->window_height()});

  const double w_width = static_cast<double>(gui_->window_width());
  const double w_height = static_cast<double>(gui_->window_height());

  settings_box_.w = w_width / 1.6;
  settings_box_.h = w_height / 1.6;
  settings_box_.x = (w_width / 2) - (settings_box_.w / 2);
  settings_box_.y = w_height / 4.5;
  RenderImage(renderer, "assets/textures/settings_box.png", settings_box_);

  back_button_.w = w_width / 6.4;
  back_button_.h = w_height / 10;
  back_button_.x = (w_width / 2) - (back_button_.w / 2);
  back_button_.y = settings_box_.y + settings_box_.h + w_height / 24;
  RenderImage(renderer, "assets/textures/button_back.png", back_button_);

  for (size_t i = 0; i < option_list_.size(); ++i) {
    UpdateOptionPosition(i);
    RenderOption(renderer, option_list_[i]);
  }
}

void SettingsState::AddOption(const std::string& label,
                              const std::vector<std::string>& options) {
  Option option;
  option.label = label;
  option.options = options;

  if (!gui_->vsync() && label == "VSync") {
    option.current_option_index = 1;
  }

  option_list_.push_back(option);
}

void SettingsState::UpdateOptionPosition(int index) {
  const int gap = settings_box_.h / 20;
  const int option_height = settings_box_.h / 10;

  option_list_[index].label_area.h = option_height;
  option_list_[index].option_area.h = option_height;
  option_list_[index].leftarrow_area.h = option_height;
  option_list_[index].leftarrow_area.w = option_height;
  option_list_[index].rightarrow_area.h = option_height;
  option_list_[index].rightarrow_area.w = option_height;

  option_list_[index].label_area.x = settings_box_.x + settings_box_.w / 15;
  option_list_[index].option_area.x = option_list_[index].label_area.x;
  option_list_[index].leftarrow_area.x = option_list_[index].label_area.x;
  option_list_[index].rightarrow_area.x = option_list_[index].label_area.x;
  option_list_[index].label_area.y =
      settings_box_.y + gap + index * (option_height + gap);
  option_list_[index].option_area.y = option_list_[index].label_area.y;
  option_list_[index].leftarrow_area.y = option_list_[index].label_area.y;
  option_list_[index].rightarrow_area.y = option_list_[index].label_area.y;
}

void SettingsState::RenderOption(SDL_Renderer* renderer, Option& option) {
  // Render the label
  SDL_Surface* label_surface = TTF_RenderUTF8_Solid(
      gui_->jersey32(), option.label.c_str(), {255, 255, 255, 255});
  SDL_Texture* label_texture =
      SDL_CreateTextureFromSurface(renderer, label_surface);
  option.label_area.w =
      label_surface->w *
      static_cast<int>(static_cast<double>(option.label_area.h) /
                       label_surface->h);
  SDL_FreeSurface(label_surface);
  SDL_RenderCopy(renderer, label_texture, nullptr, &option.label_area);
  SDL_DestroyTexture(label_texture);

  // Render the current choice
  const int current_option_x =
      settings_box_.x + static_cast<double>(settings_box_.w) * 0.7;
  SDL_Surface* current_option_surface = TTF_RenderUTF8_Solid(
      gui_->jersey32(), option.current_option().c_str(), {255, 255, 255, 255});
  SDL_Texture* current_option_texture =
      SDL_CreateTextureFromSurface(renderer, current_option_surface);
  option.option_area.w =
      current_option_surface->w *
      static_cast<int>(static_cast<double>(option.option_area.h) /
                       current_option_surface->h);
  SDL_FreeSurface(current_option_surface);
  option.option_area.x = current_option_x - option.option_area.w / 2;
  SDL_RenderCopy(renderer, current_option_texture, nullptr,
                 &option.option_area);
  SDL_DestroyTexture(current_option_texture);

  option.leftarrow_area.x = option.option_area.x -
                            double(option.option_area.w) / 4 -
                            option.leftarrow_area.w;
  RenderImage(renderer, "assets/textures/left-arrow.png",
              option.leftarrow_area);
  option.rightarrow_area.x =
      option.option_area.x + double(option.option_area.w) * 1.25;
  RenderImage(renderer, "assets/textures/right-arrow.png",
              option.rightarrow_area);
}
}  // namespace gui
