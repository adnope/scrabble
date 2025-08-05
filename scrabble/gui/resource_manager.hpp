#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <string>
#include <unordered_map>

#include "core/dictionary.hpp"
#include "core/lexicon.hpp"


namespace gui {
class ResourceManager {
 public:
  ResourceManager() {
    // csw_lexicon_->PreloadDictionary(core::Dictionary::CSW);
    twl_lexicon_->PreloadDictionary(core::Dictionary::TWL);
  }

  ResourceManager(const ResourceManager&) = delete;
  ResourceManager(ResourceManager&&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;
  ResourceManager& operator=(ResourceManager&&) = delete;
  ~ResourceManager();

  void SetRenderer(SDL_Renderer* renderer) { renderer_ = renderer; }

  SDL_Texture* GetTexture(const std::string& path);

  SDL_Cursor* GetSystemCursor(SDL_SystemCursor id);

  TTF_Font* jersey32() const { return jersey32_; }
  TTF_Font* jersey48() const { return jersey48_; }
  TTF_Font* jersey64() const { return jersey64_; }

  core::Lexicon* csw_lexicon() { return csw_lexicon_; }
  core::Lexicon* twl_lexicon() { return twl_lexicon_; }

  void Clear();

 private:
  SDL_Renderer* renderer_{};

  TTF_Font* jersey32_ = TTF_OpenFont("assets/fonts/jersey.ttf", 32);
  TTF_Font* jersey48_ = TTF_OpenFont("assets/fonts/jersey.ttf", 48);
  TTF_Font* jersey64_ = TTF_OpenFont("assets/fonts/jersey.ttf", 64);

  std::unordered_map<std::string, SDL_Texture*> texture_cache_;
  std::unordered_map<SDL_SystemCursor, SDL_Cursor*> cursor_cache_;

  core::Lexicon* csw_lexicon_ = new core::Lexicon();
  core::Lexicon* twl_lexicon_ = new core::Lexicon();
};
}  // namespace gui
