#include "create_new_game.hpp"

#include <string>

#include "SDL_image.h"

namespace gui {
void CreateNewGameState::HandleEvent(SDL_Event& event) {
  if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
    gui_->ChangeState(GUI::GameStateType::MainMenu);
  }
}

void CreateNewGameState::Update() {
  // e.g. animate background
}

void CreateNewGameState::RenderImage(SDL_Renderer* renderer,
                                     const std::string& image_path,
                                     SDL_Rect area) {
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

void CreateNewGameState::Render(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  RenderImage(renderer, "assets/textures/settings_background.png",
              {0, 0, gui_->window_width(), gui_->window_height()});
  
}
}  // namespace gui
