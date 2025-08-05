#pragma once

#include <array>
#include <string>
#include <vector>

#include "SDL_events.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "game/game.hpp"
#include "gui.hpp"
#include "i_game_state.hpp"

namespace gui {
class IngameState : public IGameState {
 public:
  static constexpr int kBoardSize = 15;

  struct PlayerInfoBox {
    SDL_Rect box;
    std::string name;
    int score;
  };

  struct HistoryEntry {
    SDL_Rect box;
    std::string player_name;
    std::string move_type;
    std::string move_content;
  };

  struct EndgamePlayerInfo {
    std::string name;
    std::string final_score;
    std::string top_word;
    std::string top_word_points;

    EndgamePlayerInfo(const std::string& name, const std::string& final_score,
                      const std::string& top_word,
                      const std::string& top_word_points)
        : name(name),
          final_score(final_score),
          top_word(top_word),
          top_word_points(top_word_points) {}

    std::string content() const {
      if (top_word.empty()) {
        return "Score: " + final_score + ", No word placed";
      }
      return "Score: " + final_score + ", Top word: " + top_word + " (" +
             top_word_points + " points)";
    }
  };

  IngameState(GUI* gui, core::Lexicon* lexicon,
              const std::vector<std::string>& player_names);

  void HandleEvent(SDL_Event& event) override;

  void Update() override;

  void Render(SDL_Renderer* renderer) override;

 private:
  GUI* gui_;
  game::Game game_;

  std::array<std::array<SDL_Rect, kBoardSize>, kBoardSize>
      board_display_grid_{};
  std::array<std::array<bool, kBoardSize>, kBoardSize> board_occupied{};
  std::array<SDL_Rect, 7> deck_display_{};
  std::vector<PlayerInfoBox> player_infos_;
  std::vector<HistoryEntry> movehistory_;

  SDL_Rect submit_button_{};
  SDL_Rect swap_button_{};
  SDL_Rect pass_button_{};

  SDL_Rect movehistory_box_{};

  bool dragging_tile_ = false;
  int dragged_tile_index_ = -1;
  SDL_Point drag_offset_{};
  SDL_Rect dragged_tile_rect_{};
  core::Player::Move pending_move_;

  bool show_swap_popup_ = false;
  bool no_tile_selected_error = false;
  std::vector<int> selected_swap_indices_;
  std::array<SDL_Rect, 7> swap_deck_{};
  SDL_Rect swap_popup_box_{};
  SDL_Rect swap_confirm_button_{};
  SDL_Rect swap_cancel_button_{};

  bool show_select_for_blank_popup = false;
  int selected_letter_index_ = -1;
  char blank_tile_use_ = '?';
  int blank_tile_index_ = -1;
  int blank_tile_row_ = -1;
  int blank_tile_col_ = -1;
  std::array<SDL_Rect, 26> letter_rects_{};
  SDL_Rect select_for_blank_box_{};

  bool game_over = false;
  SDL_Rect endgame_popup_box_{};
  SDL_Rect back_button_{};
  std::vector<EndgamePlayerInfo> endgame_info_;

  void ClearPendingPlayerMove();

  void InitFirstPlayer();
  void InitPlayerDecks();

  void UpdateBoardSize();
  void RenderBoard(SDL_Renderer* renderer);

  void UpdateDeckSize();
  void RenderDeck(SDL_Renderer* renderer);

  void UpdatePlayerinfoBoxesSize();
  void RenderPlayerinfoBoxes(SDL_Renderer* renderer);

  void UpdateActionButtonsSize();
  void RenderActionButtons(SDL_Renderer* renderer);

  void UpdateMoveHistorySize();
  void RenderMoveHistory(SDL_Renderer* renderer);

  void RenderDraggedTile(SDL_Renderer* renderer);

  void RenderSwapPopup(SDL_Renderer* renderer);
  void RenderBlankSelectPopup(SDL_Renderer* renderer);
  void RenderEndgamePopup(SDL_Renderer* renderer);

  void HandleTileDrag(SDL_Event& event);
  void HandleBoardSquareClick(SDL_Event& event);
  void HandleActionButtons(SDL_Event& event);
  void HandleSwapPopupEvent(SDL_Event& event);
  void HandleBlankSelectPopupEvent(SDL_Event& event);
  void HandleEndgamePopupEvent(SDL_Event& event);

  void SubmitMove();

  void UpdateEndgameInfo();
};
}  // namespace gui
