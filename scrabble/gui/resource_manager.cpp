#include "resource_manager.hpp"

#include <iostream>

namespace gui {
ResourceManager::~ResourceManager() { Clear(); }

SDL_Texture* ResourceManager::GetTexture(const std::string& path) {
  if (texture_cache_.find(path) != texture_cache_.end()) {
    return texture_cache_[path];
  }

  SDL_Surface* surface = IMG_Load(path.c_str());
  if (surface == nullptr) {
    std::cerr << "IMG_Load failed for " << path << ": " << IMG_GetError()
              << '\n';
    return nullptr;
  }

  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
  SDL_FreeSurface(surface);

  if (texture == nullptr) {
    std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError()
              << '\n';
    return nullptr;
  }

  texture_cache_[path] = texture;
  return texture;
}

SDL_Cursor* ResourceManager::GetSystemCursor(SDL_SystemCursor id) {
  auto it = cursor_cache_.find(id);
  if (it != cursor_cache_.end()) {
    return it->second;
  }

  SDL_Cursor* cursor = SDL_CreateSystemCursor(id);
  if (cursor == nullptr) {
    std::cerr << "SDL_CreateSystemCursor failed: " << SDL_GetError() << '\n';
    return nullptr;
  }

  cursor_cache_[id] = cursor;
  return cursor;
}

void ResourceManager::Clear() {
  for (auto& pair : texture_cache_) {
    SDL_DestroyTexture(pair.second);
  }
  texture_cache_.clear();

  for (auto& pair : cursor_cache_) {
    SDL_FreeCursor(pair.second);
  }
  cursor_cache_.clear();
  delete csw_lexicon_;
  delete twl_lexicon_;
}
}  // namespace gui
