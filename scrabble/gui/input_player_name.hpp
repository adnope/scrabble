// #pragma once

// #include "SDL_events.h"
// #include "SDL_rect.h"
// #include "game_state.hpp"
// #include "gui.hpp"

// namespace gui {
// class InputPlayerName : public IGameState {
//     public:
//         explicit InputPlayerName(GUI* gui) : gui_(gui) {}

//         void HandleEvent(SDL_Event& event) override;

//         void Update() override;

//         void Render(SDL_Renderer* renderer) override;

//     private:
//         GUI* gui_;

//         static void RenderImage(SDL_Renderer* renderer, const std::string& image_path,
//                                 SDL_Rect button);

//         void HandleButtonEvent(SDL_Event& event);
//         };
// }  // namespace gui
