// #include "input_player_name.hpp"

// #include <string>

// #include "SDL_image.h"
// #include "SDL_mouse.h"
// #include "gui.hpp"

// namespace gui {
// void InputPlayerName::HandleButtonEvent(SDL_Event& event) {
//   SDL_Point mouse_pos;

//   if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN) {
//     mouse_pos.x = event.motion.x;
//     mouse_pos.y = event.motion.y;

//     bool is_hovering_newgame = SDL_PointInRect(&mouse_pos, &newgame_button_);
//     bool is_hovering_settings = SDL_PointInRect(&mouse_pos, &settings_button_);
//     bool is_hovering_quit = SDL_PointInRect(&mouse_pos, &quit_button_);

//     if (is_hovering_newgame || is_hovering_settings || is_hovering_quit) {
//       SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
//     } else {
//       SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
//     }

//     if ((is_hovering_newgame || is_hovering_settings || is_hovering_quit) &&
//         event.type == SDL_MOUSEBUTTONDOWN &&
//         event.button.button == SDL_BUTTON_LEFT) {
//       if (is_hovering_newgame) {
//         SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
//         gui_->ChangeState(GUI::GameStateType::SelectNumPlayers);
//       }
//       if (is_hovering_settings) {
//         SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
//         gui_->ChangeState(GUI::GameStateType::Settings);
//       }
//       if (is_hovering_quit) {
//         SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
//         gui_->Quit();
//       }
//     }
//   }
// }

// void InputPlayerName::HandleEvent(SDL_Event& event) { HandleButtonEvent(event); }

// void InputPlayerName::Update() {
//   // e.g. animate background
// }

// void InputPlayerName::Render(SDL_Renderer* renderer) {
//   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//   SDL_RenderClear(renderer);

 
// }

// void InputPlayerName::RenderImage(SDL_Renderer* renderer,
//                                 const std::string& image_path, SDL_Rect area) {
//   SDL_Surface* surface = IMG_Load(image_path.c_str());
//   if (surface == nullptr) {
//     SDL_Log("IMG_Load failed for %s: %s", image_path.c_str(), IMG_GetError());
//     return;
//   }
//   SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
//   SDL_FreeSurface(surface);

//   if (texture == nullptr) {
//     SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
//     return;
//   }

//   SDL_RenderCopy(renderer, texture, nullptr, &area);
//   SDL_DestroyTexture(texture);
// }
// }  // namespace gui
