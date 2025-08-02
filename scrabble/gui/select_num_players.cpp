#include "select_num_players.hpp"

#include <string>

#include "SDL_image.h"

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
      SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
      if (event.type == SDL_MOUSEBUTTONDOWN &&
          event.button.button == SDL_BUTTON_LEFT) {
        if (is_hovering_back) {
          SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
          gui_->ChangeState(GUI::GameStateType::MainMenu);
        }
        if (is_hovering_two) {
        }
        if (is_hovering_three) {
        }
        if (is_hovering_four) {
        }
      }
    } else {
      SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
    }
  }
}

void SelectNumPlayersState::Update() {
  // e.g. animate background
}

void SelectNumPlayersState::RenderImage(SDL_Renderer* renderer,
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

void SelectNumPlayersState::Render(SDL_Renderer* renderer) {
  SDL_RenderClear(renderer);

  const double w_width = static_cast<double>(gui_->window_width());
  const double w_height = static_cast<double>(gui_->window_height());

  int logo_w = static_cast<int>(w_width / 1.5);
  int logo_h = static_cast<int>(w_height / 6);
  int logo_x = w_width / 2 - (static_cast<double>(logo_w) / 2);
  int logo_y = static_cast<int>(w_height / 10);
  RenderImage(renderer, "assets/textures/chooseplayer.png",
              {logo_x, logo_y, logo_w, logo_h});

  int button_size = static_cast<int>(w_height / 4);
  int button_y = (w_height / 2) - (static_cast<double>(button_size) / 2);
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

  RenderImage(renderer, "assets/textures/2.png", two_players_button_);
  RenderImage(renderer, "assets/textures/3.png", three_players_button_);
  RenderImage(renderer, "assets/textures/4.png", four_players_button_);
  RenderImage(renderer, "assets/textures/home.png", back_button_);
}
}  // namespace gui
