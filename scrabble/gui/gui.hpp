#pragma once

#include <cstdint>
#include <memory>

#include "SDL_render.h"
#include "SDL_video.h"
#include "game_state.hpp"

namespace gui {
class GUI {
 public:
  enum class GameStateType : uint8_t {
    MainMenu,
    CreateNewGame,
    Settings,
    EndGame
  };

  static constexpr int kInitialWindowWidth = 1280;
  static constexpr int kInitialWindowHeight = 720;

  GUI();

  GUI(const GUI&) = delete;
  GUI(GUI&&) = delete;
  GUI& operator=(const GUI&) = delete;
  GUI& operator=(GUI&&) = delete;

  ~GUI();

  void Start();

  void ChangeState(GameStateType state_type);

  void Quit() { quit = true; }

  int window_width() const { return window_width_; }
  int window_height() const { return window_height_; }

 private:
  int window_width_ = kInitialWindowWidth;
  int window_height_ = kInitialWindowHeight;
  SDL_Window* window_{};
  SDL_Renderer* renderer_{};
  bool quit = false;

  std::unique_ptr<IGameState> current_state_;
  GameStateType current_state_type_ = GUI::GameStateType::MainMenu;

  bool Init();
};
}  // namespace gui
