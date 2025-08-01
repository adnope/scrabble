#include "gui.hpp"

#include <iostream>
#include <memory>

#include "SDL2/SDL.h"
#include "SDL_error.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "select_num_players.hpp"
#include "main_menu.hpp"
#include "settings.hpp"

namespace gui {
GUI::GUI() { current_state_ = std::make_unique<MainMenuState>(this); }

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

  window_ = SDL_CreateWindow(
      "Scrabble", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width_,
      window_height_,
      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  if (window_ == nullptr) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError()
              << '\n';
    return false;
  }
  SetSize(GetScreenWidth(), GetScreenHeight());
  SDL_SetWindowMinimumSize(window_, kInitialWindowWidth, kInitialWindowHeight);
  SDL_SetWindowMaximumSize(window_, window_width_, window_height_);

  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
  if (renderer_ == nullptr) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError()
              << '\n';
    return false;
  }

  aptos32_ = TTF_OpenFont("assets/fonts/Aptos.ttf", 32);
  if (aptos32_ == nullptr) {
    SDL_Log("Failed to load font: %s", TTF_GetError());
  }
  jersey32_ = TTF_OpenFont("assets/fonts/Jersey.ttf", 32);
  if (jersey32_ == nullptr) {
    SDL_Log("Failed to load font: %s", TTF_GetError());
  }

  return true;
}

void GUI::ChangeState(GUI::GameStateType state_type) {
  if (state_type == current_state_type_) {
    return;
  }
  current_state_type_ = state_type;

  switch (state_type) {
    case GameStateType::MainMenu:
      current_state_ = std::make_unique<MainMenuState>(this);
      break;
    case GameStateType::SelectNumPlayers:
      current_state_ = std::make_unique<SelectNumPlayersState>(this);
      break;
    case GameStateType::Settings:
      current_state_ = std::make_unique<SettingsState>(this);
      break;
    case GameStateType::EndGame:
      break;
  }
}

void GUI::Start() {
  if (!Init()) {
    std::cerr << "Init SDL failed! Error: \n"
              << SDL_GetError() << '\n'
              << IMG_GetError() << '\n'
              << TTF_GetError() << '\n';
  }
  SDL_Event e;
  while (!quit_) {
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
      SDL_Delay(8);
    }
  }
}
}  // namespace gui
