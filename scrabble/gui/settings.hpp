#pragma once

#include <string>
#include <vector>

#include "SDL_rect.h"
#include "game_state.hpp"
#include "gui.hpp"

namespace gui {
class SettingsState : public IGameState {
 public:
  struct Option {
    std::string label;
    SDL_Rect rightarrow_area;
    SDL_Rect leftarrow_area;
    SDL_Rect option_area;
    std::vector<std::string> options;
    int current_option_index = 0;

    const std::string& current_option() const {
      return options[current_option_index];
    }
  };

  explicit SettingsState(GUI* gui) : gui_(gui) {
    AddOption("Dictionary",{"CSW", "TWL"});
    AddOption("Resolution",{"1280x720", "1920x1080"});
    AddOption("VSync",{"On", "Off"});
  }

  void HandleEvent(SDL_Event& event) override;

  void Update() override;

  void Render(SDL_Renderer* renderer) override;

 private:
  GUI* gui_;
  std::vector<Option> option_list_;
  SDL_Rect savesetting_{};
  SDL_Rect home_button_{};
  SDL_Rect setting_{};
  SDL_Rect setting_box_{};

  static void RenderImage(SDL_Renderer* renderer, const std::string& image_path,
                          SDL_Rect area);

  void HandleArrows(SDL_Event& event);

  void RenderOption(SDL_Renderer* renderer, Option& option);

  void AddOption(const std::string& label, const std::vector<std::string>& options);

  void UpdateOptionPosition(int index);
};
}  // namespace gui
