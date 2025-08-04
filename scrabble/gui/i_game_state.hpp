#pragma once

#include "SDL_events.h"
#include "SDL_render.h"

namespace gui {
class IGameState {
 public:
  IGameState() = default;
  virtual ~IGameState() = default;
  IGameState(const IGameState&) = default;
  IGameState(IGameState&&) = default;
  IGameState& operator=(const IGameState&) = default;
  IGameState& operator=(IGameState&&) = default;

  virtual void HandleEvent(SDL_Event& event) = 0;
  virtual void Update() = 0;
  virtual void Render(SDL_Renderer* renderer) = 0;
};
}  // namespace gui
