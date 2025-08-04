#include "select_num_players_state.hpp"

#include <string>

#include "SDL_mouse.h"
#include "gui.hpp"

namespace gui {
void SelectNumPlayersState::HandleEvent(SDL_Event& event) {
  SDL_Point mouse_pos;
  if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN) {
    mouse_pos.x = event.motion.x;
    mouse_pos.y = event.motion.y;

    bool is_hovering_back = SDL_PointInRect(&mouse_pos, &back_button_) == 1;
    bool is_hovering_two =
        SDL_PointInRect(&mouse_pos, &two_players_button_) == 1;
    bool is_hovering_three =
        SDL_PointInRect(&mouse_pos, &three_players_button_) == 1;
    bool is_hovering_four =
        SDL_PointInRect(&mouse_pos, &four_players_button_) == 1;

    if (is_hovering_back || is_hovering_two || is_hovering_three ||
        is_hovering_four) {
      SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_HAND));
      if (event.type == SDL_MOUSEBUTTONDOWN &&
          event.button.button == SDL_BUTTON_LEFT) {
        if (is_hovering_back) {
          SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_ARROW));
          gui_->ChangeState(GUI::GameStateType::MainMenu);
        }
        if (is_hovering_two) {
          gui_->ChangeState(GUI::GameStateType::InputNames, 2);
        }
        if (is_hovering_three) {
          gui_->ChangeState(GUI::GameStateType::InputNames, 3);
        }
        if (is_hovering_four) {
          gui_->ChangeState(GUI::GameStateType::InputNames, 4);
        }
      }
    } else {
      SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_ARROW));
    }
  }
}

void SelectNumPlayersState::Update() {
  // e.g. animate background
}

void SelectNumPlayersState::Render(SDL_Renderer* renderer) {
  SDL_RenderClear(renderer);

  int w_width = gui_->window_width();
  int w_height = gui_->window_height();

  int logo_w = w_width / 3 * 2;
  int logo_h = w_height / 6;
  int logo_x = (w_width / 2) - (logo_w / 2);
  int logo_y = w_height / 10;
  gui_->RenderImage(renderer, "assets/textures/chooseplayer.png",
                    {logo_x, logo_y, logo_w, logo_h});

  int button_size = w_height / 4;
  int button_y = (w_height / 2) - (button_size / 2);
  int gap = (w_width - 3 * button_size) / 4;
  two_players_button_ = {gap, button_y, button_size, button_size};
  three_players_button_ = {(gap * 2) + button_size, button_y, button_size,
                           button_size};
  four_players_button_ = {(gap * 3) + (2 * button_size), button_y, button_size,
                          button_size};

  back_button_.w = w_height / 12;
  back_button_.h = w_height / 12;
  back_button_.x = w_width / 50;
  back_button_.y = w_width / 50;

  gui_->RenderImage(renderer, "assets/textures/2.png", two_players_button_);
  gui_->RenderImage(renderer, "assets/textures/3.png", three_players_button_);
  gui_->RenderImage(renderer, "assets/textures/4.png", four_players_button_);
  gui_->RenderImage(renderer, "assets/textures/home.png", back_button_);
}
}  // namespace gui
