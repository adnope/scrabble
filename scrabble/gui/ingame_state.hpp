#pragma once

#include <array>
#include <cstdint>
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

  enum class MoveType : uint8_t { PASS, SWAP, SUBMIT };

  struct PlayerInfoBox {
    SDL_Rect box;
    std::string name;
    int score;
  };

  struct HistoryEntry {
    SDL_Rect box;
    std::string player_name;
    MoveType move_type;
    std::string move_content;
    std::vector<core::Word> words;
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

  IngameState(GUI* gui, const std::vector<std::string>& player_names);

  void HandleEvent(SDL_Event& event) override;

  void Update() override;

  void Render(SDL_Renderer* renderer) override;

 private:
  GUI* gui_;
  game::Game game_;

  bool is_hovering_ = false;
  void HandleHovering(SDL_Event& event);

  std::array<std::array<SDL_Rect, kBoardSize>, kBoardSize>
      board_display_grid_{};
  std::array<std::array<bool, kBoardSize>, kBoardSize> board_occupied{};
  void UpdateBoardSize();
  void RenderBoard(SDL_Renderer* renderer);

  std::array<SDL_Rect, 7> deck_display_{};
  void UpdateDeckSize();
  void RenderDeck(SDL_Renderer* renderer);

  std::vector<PlayerInfoBox> player_infos_;
  void UpdatePlayerinfoBoxes();
  void RenderPlayerinfoBoxes(SDL_Renderer* renderer);

  SDL_Rect submit_button_{};
  SDL_Rect swap_button_{};
  SDL_Rect pass_button_{};
  bool ValidateMove();
  void SubmitMove();
  void UpdateActionButtonsSize();
  void RenderActionButtons(SDL_Renderer* renderer);
  void HandleActionButtons(SDL_Event& event);
  std::string submit_error_message;

  std::vector<HistoryEntry> movehistory_;
  bool history_view_locked = false;
  std::array<int, 2> visible_entries_indices_{0, 1};
  SDL_Rect history_left_arrow_{};
  SDL_Rect history_right_arrow_{};
  SDL_Rect movehistory_box_{};
  void UpdateMoveHistorySize();
  void RenderMoveHistory(SDL_Renderer* renderer);
  void HandleMoveHistoryNavigation(SDL_Event& event);

  bool dragging_tile_ = false;
  int dragged_tile_index_ = -1;
  SDL_Point drag_offset_{};
  SDL_Rect dragged_tile_rect_{};
  core::Player::Move pending_move_;
  void ClearPendingPlayerMove();
  void RenderDraggedTile(SDL_Renderer* renderer);
  void HandleTileDrag(SDL_Event& event);
  void HandleBoardSquareClick(SDL_Event& event);

  bool show_swap_popup_ = false;
  bool no_tile_selected_error = false;
  std::vector<int> selected_swap_indices_;
  std::array<SDL_Rect, 7> swap_deck_{};
  SDL_Rect swap_popup_box_{};
  SDL_Rect swap_confirm_button_{};
  SDL_Rect swap_cancel_button_{};
  void RenderSwapPopup(SDL_Renderer* renderer);
  void HandleSwapPopupEvent(SDL_Event& event);

  bool show_select_for_blank_popup = false;
  int selected_letter_index_ = -1;
  char blank_tile_use_ = '?';
  int blank_tile_index_ = -1;
  int blank_tile_row_ = -1;
  int blank_tile_col_ = -1;
  std::array<SDL_Rect, 26> letter_rects_{};
  SDL_Rect select_for_blank_box_{};
  void RenderBlankSelectPopup(SDL_Renderer* renderer);
  void HandleBlankSelectPopupEvent(SDL_Event& event);

  bool game_over = false;
  SDL_Rect endgame_popup_box_{};
  SDL_Rect back_button_{};
  std::vector<EndgamePlayerInfo> endgame_info_;
  void UpdateEndgameInfo();
  void RenderEndgamePopup(SDL_Renderer* renderer);
  void HandleEndgamePopupEvent(SDL_Event& event);

  void InitFirstPlayer();
  SDL_Rect init_first_player_popup{};
  bool showing_first_player_popup_ = true;
  uint32_t first_player_popup_start_time_ = 0;
  int first_player_popup_stage = -1;
  std::vector<char> init_letters;
  void RenderFirstPlayerInitPopup(SDL_Renderer* renderer);

  void InitPlayerDecks();
};
}  // namespace gui
