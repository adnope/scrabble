#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <future>
#include <string>
#include <unordered_map>

#include "core/dictionary.hpp"
#include "core/lexicon.hpp"

namespace gui {
class ResourceManager {
 public:
  ResourceManager() : csw_loading_(true), twl_loading_(true) {
    // csw_lexicon_->PreloadDictionary(core::Dictionary::CSW);
    // twl_lexicon_->PreloadDictionary(core::Dictionary::TWL);

    csw_future_ = std::async(std::launch::async, [this] {
      csw_lexicon_->PreloadDictionary(core::Dictionary::CSW);
      csw_loading_ = false;
    });

    twl_future_ = std::async(std::launch::async, [this] {
      twl_lexicon_->PreloadDictionary(core::Dictionary::TWL);
      twl_loading_ = false;
    });
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

  bool IsCSWReady() const { return !csw_loading_; }
  bool IsTWLReady() const { return !twl_loading_; }
  core::Lexicon* csw_lexicon() { return IsCSWReady() ? csw_lexicon_ : nullptr; }
  core::Lexicon* twl_lexicon() { return IsTWLReady() ? twl_lexicon_ : nullptr; }

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

  std::future<void> csw_future_;
  std::future<void> twl_future_;
  std::atomic<bool> csw_loading_{false};
  std::atomic<bool> twl_loading_{false};
};
}  // namespace gui
