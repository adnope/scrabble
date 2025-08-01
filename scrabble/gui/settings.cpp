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

    bool is_hovering_back = SDL_PointInRect(&mouse_pos, &home_button_);

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
  SDL_RenderClear(renderer);

  const double w_width = static_cast<double>(gui_->window_width());
  const double w_height = static_cast<double>(gui_->window_height());
  // load setting letter
  SDL_Surface* setting_surface =
IMG_Load("assets/textures/Setting.png");
  if (setting_surface) {
    setting_.w = setting_surface->w;
    setting_.h = setting_surface->h;
    SDL_FreeSurface(setting_surface);
  } else {
    setting_.w = w_width / 6.4;
    setting_.h = w_height / 10;
  }
  setting_.x = w_width/ 2 - (setting_.w / 2);
  setting_.y = 0;
  RenderImage(renderer, "assets/textures/Setting.png", setting_);

  // load settings box (edit.png)
  SDL_Surface* settings_box_surface = IMG_Load("assets/textures/edit.png");
  if (settings_box_surface) {
    setting_box_.w = settings_box_surface->w;
    setting_box_.h = settings_box_surface->h;
    SDL_FreeSurface(settings_box_surface);
  } else {
    setting_box_.w = std::max(400.0, std::min(w_width / 1.6, w_width - 40));
    setting_box_.h = std::max(300.0, std::min(w_height / 1.6, w_height - 40));
  }
  setting_box_.x = (w_width / 2) - (setting_box_.w / 2);
  setting_box_.y = w_height * 0.5 - setting_box_.h / 2;
  RenderImage(renderer, "assets/textures/edit.png", setting_box_);
  // Load the settings box image to get its actual size, similar to home button
  SDL_Surface* settings_img_surface = IMG_Load("assets/textures/savesetting.png");
  if (settings_img_surface) {
    savesetting_.w = settings_img_surface->w;
    savesetting_.h = settings_img_surface->h;
    SDL_FreeSurface(settings_img_surface);
  } else {
    // Fallback to proportional sizing if image fails to load
    savesetting_.w = std::max(400.0, std::min(w_width / 1.6, w_width - 40));
    savesetting_.h = std::max(300.0, std::min(w_height / 1.6, w_height - 40));
  }
  savesetting_.x = (w_width / 2) - (savesetting_.w / 2);
  savesetting_.y = w_height * 0.85;
  RenderImage(renderer, "assets/textures/savesetting.png", savesetting_);

  // Load the back button image to get its actual size
  SDL_Surface* back_img_surface = IMG_Load("assets/textures/home.png");
  if (back_img_surface) {
    home_button_.w = back_img_surface->w;
    home_button_.h = back_img_surface->h;
    SDL_FreeSurface(back_img_surface);
  } else {
    // Fallback to previous sizing if image fails to load
    home_button_.w = w_width / 6.4;
    home_button_.h = w_height / 10;
  }
  home_button_.x = 0;
  home_button_.y = 0;
  RenderImage(renderer, "assets/textures/home.png", home_button_);

  for (size_t i = 0; i < option_list_.size(); ++i) {
    UpdateOptionPosition(i);
    RenderOption(renderer, option_list_[i]);
  }
}

void SettingsState::AddOption(const std::string& label, const std::vector<std::string>& options) {
  Option option;
  option.label = label;
  option.options = options;

  if (!gui_->vsync() && label == "VSync") {
    option.current_option_index = 1;
  }

  option_list_.push_back(option);
}

void SettingsState::UpdateOptionPosition(int index) {
    Option& option = option_list_[index];
    const double w_width = static_cast<double>(gui_->window_width());
    const double w_height = static_cast<double>(gui_->window_height());

    // Set arrow sizes for best look (slightly larger and proportional)
    option.rightarrow_area.w = w_width / 18;
    option.rightarrow_area.h = w_height / 16;
    option.leftarrow_area.w = w_width / 18;
    option.leftarrow_area.h = w_height / 16;

    // Set option area size for better appearance
    option.option_area.w = 150;
    option.option_area.h = 150;

    option.rightarrow_area.x = (w_width / 2) + (option.option_area.w / 2)+400 ;
    option.leftarrow_area.x = (w_width / 2) - (option.option_area.w / 2)+100 ;
    
    option.rightarrow_area.y = (w_height * (0.3 + index * 0.1)) - 
                              (option.rightarrow_area.h / 2);

    option.leftarrow_area.y = option.rightarrow_area.y;

    option.option_area.x = option.rightarrow_area.x - option.option_area.w - ;
    
    option.option_area.y = (w_height * (0.3 + index * 0.1)) - 
                          (option.option_area.h / 2);
  }
void SettingsState::RenderOption(SDL_Renderer* renderer, Option& option) {
  SDL_Color text_color = {0, 255, 0, 255}; // Green color for text
  TTF_Font* font = gui_->aptos32();



  // Render the current option
  SDL_Surface* option_surface = TTF_RenderText_Solid(font, option.current_option().c_str(), text_color);
  SDL_Texture* option_texture = SDL_CreateTextureFromSurface(renderer, option_surface);
  SDL_RenderCopy(renderer, option_texture, nullptr, &option.option_area);
  SDL_FreeSurface(option_surface);
  SDL_DestroyTexture(option_texture);

  // Render arrows
  RenderImage(renderer, "assets/textures/right-arrow.png", option.rightarrow_area);
  RenderImage(renderer, "assets/textures/left-arrow.png", option.leftarrow_area);
}
} // namespace gui
