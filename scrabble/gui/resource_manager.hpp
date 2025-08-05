#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <string>
#include <unordered_map>

namespace gui {
class ResourceManager {
 public:
  ResourceManager() {}

  ResourceManager(const ResourceManager&) = default;
  ResourceManager(ResourceManager&&) = delete;
  ResourceManager& operator=(const ResourceManager&) = default;
  ResourceManager& operator=(ResourceManager&&) = delete;
  ~ResourceManager();

  void SetRenderer(SDL_Renderer* renderer) { renderer_ = renderer; }

  SDL_Texture* GetTexture(const std::string& path);

  SDL_Cursor* GetSystemCursor(SDL_SystemCursor id);

  TTF_Font* jersey32() const { return jersey32_; }
  TTF_Font* jersey48() const { return jersey48_; }
  TTF_Font* jersey64() const { return jersey64_; }

  void Clear();

 private:
  SDL_Renderer* renderer_{};

  TTF_Font* jersey32_ = TTF_OpenFont("assets/fonts/jersey.ttf", 32);
  TTF_Font* jersey48_ = TTF_OpenFont("assets/fonts/jersey.ttf", 48);
  TTF_Font* jersey64_ = TTF_OpenFont("assets/fonts/jersey.ttf", 64);

  std::unordered_map<std::string, SDL_Texture*> texture_cache_;
  std::unordered_map<SDL_SystemCursor, SDL_Cursor*> cursor_cache_;
};
}  // namespace gui
