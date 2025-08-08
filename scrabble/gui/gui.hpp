#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "SDL_mouse.h"
#include "SDL_render.h"
#include "SDL_ttf.h"
#include "SDL_video.h"
#include "core/dictionary.hpp"
#include "core/lexicon.hpp"
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

  static void RenderTextCenteredX(SDL_Renderer* renderer, TTF_Font* font,
                                  const std::string& text, int anchor_x, int y,
                                  SDL_Color color);

  static void RenderFixedHeightText(SDL_Renderer* renderer,
                                    const std::string& text, TTF_Font* font,
                                    int x, int y, int height, SDL_Color color);
  static void RenderFixedHeightCenteredText(SDL_Renderer* renderer,
                                            const std::string& text,
                                            TTF_Font* font, int x, int y,
                                            int height, SDL_Color color,
                                            int& width);

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
                   const std::vector<std::string>& player_names);

  void Quit() { quit_ = true; }

  int window_width() const { return window_width_; }
  int window_height() const { return window_height_; }
  bool vsync() const { return vsync_; }
  TTF_Font* jersey32() const { return resources_.jersey32(); }
  TTF_Font* jersey48() const { return resources_.jersey48(); }
  TTF_Font* jersey64() const { return resources_.jersey64(); }
  core::Lexicon* lexicon() { return lexicon_; }
  auto lexicon_type() { return lexicon_type_; }
  SDL_Cursor* cursor(SDL_SystemCursor cursor) {
    return resources_.GetSystemCursor(cursor);
  }
  std::string current_resolution() { return current_resolution_; }

  void SetSize(const int width, const int height) {
    if (width == 1280 && height == 720) {
      window_width_ = kInitialWindowWidth;
      window_height_ = kInitialWindowHeight;
      SDL_SetWindowSize(window_, width, height);
      current_resolution_ = "1280x720";
      return;
    }
    window_width_ = width, window_height_ = height;
    current_resolution_ = std::to_string(width) + "x" + std::to_string(height);
    SDL_SetWindowSize(window_, width, height);
  }

  void SetVsync(bool on) {
    vsync_ = on;
    SDL_RenderSetVSync(renderer_, static_cast<int>(vsync_));
  }

  void SetLexiconType(core::Dictionary::DictionaryType type) {
    lexicon_type_ = type;
  }

 private:
  int window_width_ = kInitialWindowWidth;
  int window_height_ = kInitialWindowHeight;
  SDL_Window* window_{};
  SDL_Renderer* renderer_{};
  std::unique_ptr<IGameState> current_state_;
  GameStateType current_state_type_ = GUI::GameStateType::MainMenu;

  bool vsync_ = false;
  std::string current_resolution_ = "1280x720";
  std::string current_lexicon_option_ = "TWL";

  bool quit_ = false;
  core::Lexicon* lexicon_ = nullptr;
  core::Dictionary::DictionaryType lexicon_type_ = core::Dictionary::TWL;

  ResourceManager resources_;

  bool Init();
};
}  // namespace gui
