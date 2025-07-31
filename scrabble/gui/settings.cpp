#include "settings.hpp"

#include <string>

#include "SDL_image.h"

namespace gui {
void Settings::HandleEvent(SDL_Event& event) {}

void Settings::Update() {
  // e.g. animate background
}

void Settings::RenderImage(SDL_Renderer* renderer,
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

void Settings::Render(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  RenderImage(renderer, "assets/textures/settings_background_2.png",
              {0, 0, gui_->window_width(), gui_->window_height()});

  const double w_width = static_cast<double>(gui_->window_width());
  const double w_height = static_cast<double>(gui_->window_height());

  settings_box.w = w_width / 1.6;
  settings_box.h = w_height / 1.6;
  settings_box.x = (w_width / 2) - (settings_box.w / 2);
  settings_box.y = w_height / 4.5;
  RenderImage(renderer, "assets/textures/settings_box.png", settings_box);

  back_button.w = w_width / 16;
  back_button.h = w_height / 10;
  back_button.x = (w_width / 2) - (back_button.w / 2);
  back_button.y = settings_box.y + settings_box.h + w_height / 24;
  RenderImage(renderer, "assets/textures/back_button.png", back_button);
}
}  // namespace gui
