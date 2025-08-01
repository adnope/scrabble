#include "main_menu.hpp"

#include <string>

#include "SDL_image.h"
#include "SDL_mouse.h"
#include "gui.hpp"

namespace gui {
void MainMenuState::HandleButtonEvent(SDL_Event& event) {
  SDL_Point mouse_pos;

  if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN) {
    mouse_pos.x = event.motion.x;
    mouse_pos.y = event.motion.y;

    bool is_hovering_newgame = SDL_PointInRect(&mouse_pos, &newgame_button_);
    bool is_hovering_settings = SDL_PointInRect(&mouse_pos, &settings_button_);
    bool is_hovering_quit = SDL_PointInRect(&mouse_pos, &quit_button_);

    if (is_hovering_newgame || is_hovering_settings || is_hovering_quit) {
      SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
    } else {
      SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
    }

    if ((is_hovering_newgame || is_hovering_settings || is_hovering_quit) &&
        event.type == SDL_MOUSEBUTTONDOWN &&
        event.button.button == SDL_BUTTON_LEFT) {
      if (is_hovering_newgame) {
        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
        gui_->ChangeState(GUI::GameStateType::SelectNumPlayers);
      }
      if (is_hovering_settings) {
        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
        gui_->ChangeState(GUI::GameStateType::Settings);
      }
      if (is_hovering_quit) {
        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
        gui_->Quit();
      }
    }
  }
}

void MainMenuState::HandleEvent(SDL_Event& event) { HandleButtonEvent(event); }

void MainMenuState::Update() {
  // e.g. animate background
}

void MainMenuState::Render(SDL_Renderer* renderer) {
  SDL_RenderClear(renderer);

  const double w_width = static_cast<double>(gui_->window_width());
  const double w_height = static_cast<double>(gui_->window_height());
  RenderImage(renderer, "assets/textures/MainmenuScene.png",
              {0, 0, gui_->window_width(), gui_->window_height()});

  // Load the image to get its size
  SDL_Surface* newgame_surface = IMG_Load("assets/textures/startbutton.png");
  if (newgame_surface) {
    newgame_button_.w = newgame_surface->w;
    newgame_button_.h = newgame_surface->h;
    SDL_FreeSurface(newgame_surface);
  } else {
    newgame_button_.w = static_cast<int>(w_width / 3);
    newgame_button_.h = static_cast<int>(w_height / 12);
  }
  // Set settings_button_ size based on its image, fallback to newgame_button_ size
  SDL_Surface* settings_surface = IMG_Load("assets/textures/settingicon.png");
  if (settings_surface) {
    settings_button_.w = settings_surface->w;
    settings_button_.h = settings_surface->h;
    SDL_FreeSurface(settings_surface);
  } else {
    settings_button_.w = newgame_button_.w;
    settings_button_.h = newgame_button_.h;
  }

  // Set quit_button_ size based on its image, fallback to newgame_button_ size
  SDL_Surface* quit_surface = IMG_Load("assets/textures/quitgamebutton.png");
  if (quit_surface) {
    quit_button_.w = quit_surface->w;
    quit_button_.h = quit_surface->h;
    SDL_FreeSurface(quit_surface);
  } else {
    quit_button_.w = newgame_button_.w;
    quit_button_.h = newgame_button_.h;
  }

  newgame_button_.x = static_cast<int>((w_width / 2) - (newgame_button_.w / 2));
  settings_button_.x = 0;
  quit_button_.x = newgame_button_.x;

  const int gap = static_cast<int>(w_height / 10);
  newgame_button_.y = static_cast<int>(w_height / 1.8);
  settings_button_.y = 0;
  quit_button_.y = newgame_button_.y + newgame_button_.h + gap;

  RenderImage(renderer, "assets/textures/startbutton.png", newgame_button_);
  RenderImage(renderer, "assets/textures/settingicon.png",
              settings_button_);
  RenderImage(renderer, "assets/textures/quitgamebutton.png", quit_button_);
}

void MainMenuState::RenderImage(SDL_Renderer* renderer,
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
}  // namespace gui
