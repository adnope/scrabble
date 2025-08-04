#include "main_menu_state.hpp"

#include <string>

#include "SDL_mouse.h"
#include "gui.hpp"

namespace gui {
void MainMenuState::HandleEvent(SDL_Event& event) {
  SDL_Point mouse_pos;

  if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN) {
    mouse_pos.x = event.motion.x;
    mouse_pos.y = event.motion.y;

    bool is_hovering_newgame = SDL_PointInRect(&mouse_pos, &newgame_button_);
    bool is_hovering_settings = SDL_PointInRect(&mouse_pos, &settings_button_);
    bool is_hovering_quit = SDL_PointInRect(&mouse_pos, &quit_button_);

    if (is_hovering_newgame || is_hovering_settings || is_hovering_quit) {
      SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_HAND));
    } else {
      SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_ARROW));
    }

    if ((is_hovering_newgame || is_hovering_settings || is_hovering_quit) &&
        event.type == SDL_MOUSEBUTTONDOWN &&
        event.button.button == SDL_BUTTON_LEFT) {
      if (is_hovering_newgame) {
        SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_ARROW));
        gui_->ChangeState(GUI::GameStateType::SelectNumPlayers);
      }
      if (is_hovering_settings) {
        SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_ARROW));
        gui_->ChangeState(GUI::GameStateType::Settings);
      }
      if (is_hovering_quit) {
        SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_ARROW));
        gui_->Quit();
      }
    }
  }
}

void MainMenuState::Update() {
  // e.g. animate background
}

void MainMenuState::Render(SDL_Renderer* renderer) {
  const double w_width = static_cast<double>(gui_->window_width());
  const double w_height = static_cast<double>(gui_->window_height());
  gui_->RenderImage(renderer, "assets/textures/mainmenu_background.png",
                    {0, 0, gui_->window_width(), gui_->window_height()});

  newgame_button_.w = static_cast<int>(w_width / 3);
  newgame_button_.h = static_cast<int>(w_height / 12);
  settings_button_.w = newgame_button_.h;
  settings_button_.h = settings_button_.w;
  quit_button_.w = newgame_button_.w;
  quit_button_.h = newgame_button_.h;

  newgame_button_.x = static_cast<int>(
      (w_width / 2) - (static_cast<double>(newgame_button_.w) / 2));
  quit_button_.x = newgame_button_.x;

  settings_button_.x = static_cast<int>((w_width / 50));
  settings_button_.y = settings_button_.x;

  const int gap = static_cast<int>(w_height / 20);
  newgame_button_.y = static_cast<int>(w_height / 1.8);
  quit_button_.y = newgame_button_.y + newgame_button_.h + gap;

  gui_->RenderImage(renderer, "assets/textures/button_newgame.png",
                    newgame_button_);
  gui_->RenderImage(renderer, "assets/textures/icon_settings.png",
                    settings_button_);
  gui_->RenderImage(renderer, "assets/textures/button_quit.png", quit_button_);
}
}  // namespace gui
