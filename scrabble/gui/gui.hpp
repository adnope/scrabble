#pragma once

#include <cstdint>
#include <memory>

#include "SDL_render.h"
#include "SDL_ttf.h"
#include "SDL_video.h"
#include "game_state.hpp"

namespace gui {
class GUI {
 public:
  enum class GameStateType : uint8_t {
    MainMenu,
    SelectNumPlayers,
    Settings,
    EndGame
  };

  static int GetScreenWidth() {
    SDL_DisplayMode dm;
    if (SDL_GetCurrentDisplayMode(0, &dm) == 0) {
      return dm.w;
    }
    return 0;
  }

  static int GetScreenHeight() {
    SDL_DisplayMode dm;
    if (SDL_GetCurrentDisplayMode(0, &dm) == 0) {
      return dm.h;
    }
    return 0;
  }

  static constexpr int kInitialWindowWidth = 1280;
  static constexpr int kInitialWindowHeight = 780;

  GUI();

  GUI(const GUI&) = delete;
  GUI(GUI&&) = delete;
  GUI& operator=(const GUI&) = delete;
  GUI& operator=(GUI&&) = delete;

  ~GUI();

  void Start();

  void ChangeState(GameStateType state_type);

  void Quit() { quit_ = true; }

  int window_width() const { return window_width_; }
  int window_height() const { return window_height_; }
  bool vsync() const { return vsync_; }
  TTF_Font* aptos32() const { return aptos32_; }
  TTF_Font* jersey32() const { return jersey32_; }

  void SetSize(const int width, const int height) {
    window_width_ = width, window_height_ = height;
    SDL_SetWindowSize(window_, width, height);
  }

  void ToggleVSync() {
    vsync_ = !vsync_;
    SDL_RenderSetVSync(renderer_, static_cast<int>(vsync_));
  }

 private:
  int window_width_ = kInitialWindowWidth;
  int window_height_ = kInitialWindowHeight;
  SDL_Window* window_{};
  SDL_Renderer* renderer_{};
  TTF_Font* aptos32_{};
  TTF_Font* jersey32_{};
  bool quit_ = false;
  bool vsync_ = false;

  std::unique_ptr<IGameState> current_state_;
  GameStateType current_state_type_ = GUI::GameStateType::MainMenu;

  bool Init();
};
}  // namespace gui
