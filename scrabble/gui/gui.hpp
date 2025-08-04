#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "SDL_mouse.h"
#include "SDL_render.h"
#include "SDL_ttf.h"
#include "SDL_video.h"
#include "core/dictionary.hpp"
#include "i_game_state.hpp"
#include "resource_manager.hpp"

namespace gui {
bool InitSDL2();

class GUI {
 public:
  enum class GameStateType : uint8_t {
    MainMenu,
    Settings,
    SelectNumPlayers,
    InputNames,
    Ingame,
    EndGame
  };

  static void RenderText(SDL_Renderer* renderer, const std::string& text,
                         TTF_Font* font, int x, int y, SDL_Color color);

  static constexpr int kInitialWindowWidth = 1280;
  static constexpr int kInitialWindowHeight = 780;

  GUI();

  GUI(const GUI&) = delete;
  GUI(GUI&&) = delete;
  GUI& operator=(const GUI&) = delete;
  GUI& operator=(GUI&&) = delete;

  ~GUI();

  void Start();

  void RenderImage(SDL_Renderer* renderer, const std::string& image_path,
                   SDL_Rect area);

  void ChangeState(GameStateType state_type);
  void ChangeState(GameStateType state_type, int num_players);
  void ChangeState(GameStateType state_type,
                   core::Dictionary::DictionaryType dict_type,
                   const std::vector<std::string>& player_names);

  void Quit() { quit_ = true; }

  int window_width() const { return window_width_; }
  int window_height() const { return window_height_; }
  bool vsync() const { return vsync_; }
  TTF_Font* jersey32() const { return resources_.jersey32(); }
  SDL_Cursor* cursor(SDL_SystemCursor cursor) {
    return resources_.GetSystemCursor(cursor);
  }

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
  std::unique_ptr<IGameState> current_state_;
  GameStateType current_state_type_ = GUI::GameStateType::MainMenu;

  bool quit_ = false;
  bool vsync_ = false;
  core::Dictionary::DictionaryType dictionary_ = core::Dictionary::CSW;

  ResourceManager resources_;

  bool Init();
};
}  // namespace gui
