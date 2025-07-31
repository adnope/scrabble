#include "select_num_players.hpp"

#include <string>

#include "SDL_image.h"

namespace gui {
void SelectNumPlayersState::HandleEvent(SDL_Event& event) {
  SDL_Point mouse_pos;
  if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN) {
    mouse_pos.x = event.motion.x;
    mouse_pos.y = event.motion.y;

    bool is_hovering_back = SDL_PointInRect(&mouse_pos, &back_button_);
    bool is_hovering_two = SDL_PointInRect(&mouse_pos, &two_players_button_);
    bool is_hovering_three =
        SDL_PointInRect(&mouse_pos, &three_players_button_);
    bool is_hovering_four = SDL_PointInRect(&mouse_pos, &four_players_button_);

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
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  const double w_width = static_cast<double>(gui_->window_width());
  const double w_height = static_cast<double>(gui_->window_height());

  RenderImage(renderer, "assets/textures/choose_num_players_background.png",
              {0, 0, gui_->window_width(), gui_->window_height()});

  int button_size = static_cast<int>(w_height / 3);
  int button_y = (w_height / 2) - (static_cast<double>(button_size) / 2);
  int gap = (w_width - 3 * button_size) / 4;
  two_players_button_ = {gap, button_y, button_size, button_size};
  three_players_button_ = {(gap * 2) + button_size, button_y, button_size,
                           button_size};
  four_players_button_ = {(gap * 3) + (2 * button_size), button_y, button_size,
                          button_size};
  RenderImage(renderer, "assets/textures/2_players.png", two_players_button_);
  RenderImage(renderer, "assets/textures/3_players.png", three_players_button_);
  RenderImage(renderer, "assets/textures/4_players.png", four_players_button_);

  back_button_.w = static_cast<int>(w_width / 6.4);
  back_button_.h = static_cast<int>(w_height / 10);
  back_button_.x = static_cast<int>((w_width / 2) -
                                    (static_cast<double>(back_button_.w) / 2));
  back_button_.y = static_cast<int>(w_height * 0.8);
  RenderImage(renderer, "assets/textures/button_back.png", back_button_);
}
}  // namespace gui
