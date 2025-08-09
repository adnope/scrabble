#include "gui.hpp"

#include <iostream>
#include <memory>

#include "SDL2/SDL.h"
#include "SDL_error.h"
#include "SDL_image.h"
#include "SDL_rect.h"
#include "SDL_ttf.h"
#include "SDL_video.h"
#include "core/dictionary.hpp"
#include "ingame_state.hpp"
#include "input_names_state.hpp"
#include "main_menu_state.hpp"
#include "resource_manager.hpp"
#include "select_num_players_state.hpp"
#include "settings_state.hpp"

namespace gui {
bool InitSDL2() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
              << '\n';
    return false;
  }
  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
    std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError()
              << '\n';
    return false;
  }
  if (TTF_Init() == -1) {
    std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError()
              << '\n';
    return false;
  }
  return true;
}

void GUI::RenderImage(SDL_Renderer* renderer, const std::string& image_path,
                      SDL_Rect area) {
  SDL_Texture* texture = resources_.GetTexture(image_path);
  SDL_RenderCopy(renderer, texture, nullptr, &area);
}

void GUI::RenderText(SDL_Renderer* renderer, const std::string& text,
                     TTF_Font* font, int x, int y, SDL_Color color) {
  if (text.empty()) {
    return;
  }
  if (font == nullptr) {
    std::cerr << "Font is null\n";
    return;
  }
  SDL_Surface* surface = TTF_RenderUTF8_Solid(font, text.c_str(), color);
  if (surface == nullptr) {
    std::cerr << "Failed to create surface: " << TTF_GetError() << '\n';
    return;
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == nullptr) {
    std::cerr << "Failed to create texture: " << SDL_GetError() << '\n';
    SDL_FreeSurface(surface);
    return;
  }

  SDL_Rect dst = {x, y, surface->w, surface->h};
  SDL_RenderCopy(renderer, texture, nullptr, &dst);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void GUI::RenderTextCenteredX(SDL_Renderer* renderer, TTF_Font* font,
                              const std::string& text, int anchor_x, int y,
                              SDL_Color color) {
  if (text.empty()) {
    return;
  }
  if (font == nullptr) {
    std::cerr << "Font is null\n";
    return;
  }
  SDL_Surface* surface = TTF_RenderUTF8_Solid(font, text.c_str(), color);
  if (surface == nullptr) {
    std::cerr << "Failed to create surface: " << TTF_GetError() << '\n';
    return;
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == nullptr) {
    std::cerr << "Failed to create texture: " << SDL_GetError() << '\n';
    SDL_FreeSurface(surface);
    return;
  }

  SDL_Rect area;
  area.w = surface->w;
  area.h = surface->h;
  area.x = anchor_x - (area.w / 2);
  area.y = y;
  SDL_RenderCopy(renderer, texture, nullptr, &area);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void GUI::RenderFixedHeightText(SDL_Renderer* renderer, const std::string& text,
                                TTF_Font* font, int x, int y, int height,
                                SDL_Color color) {
  if (text.empty()) {
    return;
  }
  if (font == nullptr) {
    std::cerr << "Font is null\n";
    return;
  }
  SDL_Surface* surface = TTF_RenderUTF8_Solid(font, text.c_str(), color);
  if (surface == nullptr) {
    std::cerr << "Failed to create surface: " << TTF_GetError() << '\n';
    return;
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == nullptr) {
    std::cerr << "Failed to create texture: " << SDL_GetError() << '\n';
    SDL_FreeSurface(surface);
    return;
  }

  SDL_Rect area;
  area.x = x;
  area.y = y;
  area.h = height;
  area.w = surface->w * height / surface->h;
  SDL_RenderCopy(renderer, texture, nullptr, &area);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void GUI::RenderFixedHeightTextReturnWidth(SDL_Renderer* renderer,
                                           const std::string& text,
                                           TTF_Font* font, int x, int y,
                                           int height, SDL_Color color,
                                           int& width) {
  if (text.empty()) {
    return;
  }
  if (font == nullptr) {
    std::cerr << "Font is null\n";
    return;
  }
  SDL_Surface* surface = TTF_RenderUTF8_Solid(font, text.c_str(), color);
  if (surface == nullptr) {
    std::cerr << "Failed to create surface: " << TTF_GetError() << '\n';
    return;
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == nullptr) {
    std::cerr << "Failed to create texture: " << SDL_GetError() << '\n';
    SDL_FreeSurface(surface);
    return;
  }

  SDL_Rect area;
  area.x = x;
  area.y = y;
  area.h = height;
  area.w = surface->w * height / surface->h;
  width = area.w;
  SDL_RenderCopy(renderer, texture, nullptr, &area);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void GUI::RenderFixedHeightCenteredText(SDL_Renderer* renderer,
                                        const std::string& text, TTF_Font* font,
                                        int x, int y, int height,
                                        SDL_Color color, int& width) {
  if (text.empty()) {
    return;
  }
  if (font == nullptr) {
    std::cerr << "Font is null\n";
    return;
  }
  SDL_Surface* surface = TTF_RenderUTF8_Solid(font, text.c_str(), color);
  if (surface == nullptr) {
    std::cerr << "Failed to create surface: " << TTF_GetError() << '\n';
    return;
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == nullptr) {
    std::cerr << "Failed to create texture: " << SDL_GetError() << '\n';
    SDL_FreeSurface(surface);
    return;
  }

  SDL_Rect rect;
  rect.y = y;
  rect.h = height;
  rect.w = surface->w * height / surface->h;
  rect.x = x - rect.w / 2;
  width = rect.w;
  SDL_RenderCopy(renderer, texture, nullptr, &rect);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

GUI::GUI() {
  if (!Init()) {
    std::cerr << "Init SDL failed! Error: \n"
              << SDL_GetError() << '\n'
              << IMG_GetError() << '\n'
              << TTF_GetError() << '\n';
  }
  resources_.SetRenderer(renderer_);

  // current_state_ = std::make_unique<MainMenuState>(this);
  current_state_ = std::make_unique<InputNamesState>(this, 4);
  ChangeState(GameStateType::Ingame, {"duy1", "duy2", "duy3", "duy4"});
}

GUI::~GUI() {
  if (renderer_ != nullptr) {
    SDL_DestroyRenderer(renderer_);
  }
  if (window_ != nullptr) {
    SDL_DestroyWindow(window_);
  }
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

bool GUI::Init() {
  window_ = SDL_CreateWindow(
      "Scrabble", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      kInitialWindowWidth, kInitialWindowHeight,
      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  if (window_ == nullptr) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError()
              << '\n';
    return false;
  }
  SDL_Surface* icon = IMG_Load("assets/textures/appicon.png");
  SDL_SetWindowIcon(window_, icon);
  SDL_SetWindowMinimumSize(window_, kInitialWindowWidth, kInitialWindowHeight);
  SDL_SetWindowMaximumSize(window_, 2560, 1440);

  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
  if (renderer_ == nullptr) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError()
              << '\n';
    return false;
  }

  return true;
}

void GUI::ChangeState(GUI::GameStateType state_type) {
  if (state_type == current_state_type_) {
    return;
  }

  current_state_type_ = state_type;
  if (state_type == GameStateType::MainMenu) {
    current_state_ = std::make_unique<MainMenuState>(this);
  } else if (state_type == GameStateType::SelectNumPlayers) {
    current_state_ = std::make_unique<SelectNumPlayersState>(this);
  } else if (state_type == GameStateType::Settings) {
    current_state_ = std::make_unique<SettingsState>(this);
  } else if (state_type == GameStateType::EndGame) {
  } else {
    std::cout << "Invalid state\n";
    return;
  }
}

void GUI::ChangeState(GUI::GameStateType state_type, int num_players) {
  if (state_type == current_state_type_) {
    return;
  }

  current_state_type_ = state_type;
  if (state_type == GameStateType::InputNames) {
    current_state_ = std::make_unique<InputNamesState>(this, num_players);
  } else {
    std::cout << "Invalid state\n";
    return;
  }
}

void GUI::ChangeState(GameStateType state_type,
                      const std::vector<std::string>& player_names) {
  if (state_type == current_state_type_) {
    return;
  }

  current_state_type_ = state_type;
  if (state_type == GameStateType::Ingame) {
    current_state_ = std::make_unique<IngameState>(this, player_names);
  } else {
    std::cout << "Invalid state\n";
    return;
  }
}

void GUI::Start() {
  SDL_Event e;
  while (!quit_) {
    if (lexicon_ == nullptr || lexicon_->type() != lexicon_type_) {
      if (lexicon_type_ == core::Dictionary::TWL) {
        lexicon_ = resources_.twl_lexicon();
      }
      if (lexicon_type_ == core::Dictionary::CSW) {
        lexicon_ = resources_.csw_lexicon();
      }
    }

    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit_ = true;
      } else if (e.type == SDL_WINDOWEVENT) {
        if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
          window_width_ = e.window.data1;
          window_height_ = e.window.data2;
        }
      }

      SDL_SetRenderDrawColor(renderer_, 235, 252, 255, 255);
      SDL_RenderClear(renderer_);
      current_state_->HandleEvent(e);
    }

    current_state_->Update();

    current_state_->Render(renderer_);
    SDL_RenderPresent(renderer_);
    if (!vsync_) {
      SDL_Delay(4);
    }
  }
}
}  // namespace gui
