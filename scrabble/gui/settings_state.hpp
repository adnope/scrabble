#pragma once

#include <string>
#include <vector>

#include "SDL_rect.h"
#include "core/dictionary.hpp"
#include "gui.hpp"
#include "i_game_state.hpp"

namespace gui {
class SettingsState : public IGameState {
 public:
  struct Option {
    std::string label;
    SDL_Rect label_rect{};
    SDL_Rect rightarrow_rect{};
    SDL_Rect leftarrow_rect{};
    SDL_Rect current_option_rect{};
    std::vector<std::string> options;
    int current_option_index = 0;

    const std::string& current_option() const {
      return options[current_option_index];
    }

    Option(const std::string& label, const std::vector<std::string>& options)
        : label(label), options(options) {}
  };

  explicit SettingsState(GUI* gui) : gui_(gui) {
    Option dictionary("Dictionary", {"CSW", "TWL"});
    Option resolution("Resolution", {"1280x720", "1600x900", "1920x1080"});
    Option vsync("VSync", {"On", "Off"});
    option_list_.push_back(dictionary);
    option_list_.push_back(resolution);
    option_list_.push_back(vsync);

    // Set initial options
    for (Option& option : option_list_) {
      if (option.label == "Dictionary") {
        if (gui_->lexicon_type() == core::Dictionary::TWL) {
          option.current_option_index = 1;
        }
        if (gui_->lexicon_type() == core::Dictionary::CSW) {
          option.current_option_index = 0;
        }
      }
      if (option.label == "VSync") {
        option.current_option_index = static_cast<int>(!gui_->vsync());
      }
      if (option.label == "Resolution") {
        if (gui_->current_resolution() == "1280x720") {
          option.current_option_index = 0;
        }
        if (gui_->current_resolution() == "1600x900") {
          option.current_option_index = 1;
        }
        if (gui_->current_resolution() == "1920x1080") {
          option.current_option_index = 2;
        }
      }
    }
  }

  void HandleEvent(SDL_Event& event) override;

  void Update() override;

  void Render(SDL_Renderer* renderer) override;

 private:
  GUI* gui_;
  bool is_hovering_ = false;
  void HandleHovering(SDL_Event& event);

  std::vector<Option> option_list_;
  SDL_Rect settings_box_{};
  SDL_Rect back_button_{};

  void HandleArrows(SDL_Event& event);
};
}  // namespace gui
