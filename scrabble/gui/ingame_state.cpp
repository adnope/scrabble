#include "ingame_state.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <climits>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "SDL_events.h"
#include "SDL_mouse.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_timer.h"
#include "core/board.hpp"
#include "core/dictionary.hpp"
#include "core/player.hpp"
#include "core/square.hpp"
#include "game/game.hpp"
#include "gui.hpp"

static inline SDL_Point MousePos() {
  int x = 0;
  int y = 0;
  SDL_GetMouseState(&x, &y);
  return SDL_Point{x, y};
}

static inline bool InRect(const SDL_Point& point, const SDL_Rect& rect) {
  return SDL_PointInRect(&point, &rect) == 1;
}

namespace gui {
IngameState::IngameState(GUI* gui, const std::vector<std::string>& player_names)
    : gui_(gui),
      game_(player_names),
      first_player_popup_start_time_(SDL_GetTicks()) {
  InitFirstPlayer();
  InitPlayerDecks();
  for (const auto& name : player_names) {
    player_infos_.push_back({{}, name, 0});
  }
}

void IngameState::ClearPendingPlayerMove() {
  for (const auto& [i, use, row, col] : pending_move_) {
    board_occupied.at(row).at(col) = false;
  }
  pending_move_.clear();
}

void IngameState::InitFirstPlayer() {
  auto game_bag = game_.bag();
  for (int i = 0; i < game_.num_players(); ++i) {
    char letter = game_bag.DrawTile().letter();
    init_letters.push_back(letter);
    if (letter == '?') {
      break;
    }
  }
}

void IngameState::RenderFirstPlayerInitPopup(SDL_Renderer* renderer) {
  if (!showing_first_player_popup_) {
    return;
  }

  const int modal_w = gui_->window_width() * 2 / 3;
  const int modal_h = gui_->window_height() * 2 / 3;
  init_first_player_popup = {(gui_->window_width() - modal_w) / 2,
                             (gui_->window_height() - modal_h) / 2, modal_w,
                             modal_h};

  gui_->RenderImage(renderer,
                    "assets/textures/ingame/init_first_player_background.png",
                    init_first_player_popup);

  const int padding_x = modal_w / 20;
  const int padding_y = modal_h / 40;
  GUI::RenderTextCenteredX(
      renderer, gui_->jersey64(), "Initializing first player...",
      gui_->window_width() / 2, init_first_player_popup.y + padding_y,
      {255, 255, 255, 255});

  int init_size = static_cast<int>(init_letters.size());

  for (int i = 1; i <= init_size; ++i) {
    if (SDL_GetTicks() - first_player_popup_start_time_ >
        static_cast<uint32_t>(i * 1000)) {
      first_player_popup_stage = i - 1;
    }
  }

  if (SDL_GetTicks() - first_player_popup_start_time_ >
      static_cast<uint32_t>((init_size + 1) * 1000)) {
    first_player_popup_stage = init_size;
  }

  if (first_player_popup_stage == -1) {
    return;
  }

  const int gap = modal_h / 15;
  std::array<int, 4> y{};
  y[0] = init_first_player_popup.y + (gap * 3);
  for (int i = 1; i < 4; ++i) {
    y.at(i) = y.at(i - 1) + gap * 2;
  }

  for (int i = 0; i <= first_player_popup_stage && i < init_size; ++i) {
    std::string text =
        game_.players()[i].name() + " got letter " + init_letters[i];
    GUI::RenderText(renderer, text, gui_->jersey48(),
                    padding_x + init_first_player_popup.x, y.at(i),
                    {255, 255, 255, 255});
  }

  int first_player_index = 0;
  std::string first_player_name;
  bool has_blank = false;
  for (int i = 0; i < init_size; ++i) {
    if (init_letters[i] == '?') {
      has_blank = true;
      first_player_index = i;
      first_player_name = game_.players()[first_player_index].name();
      break;
    }
  }

  if (first_player_popup_stage == init_size) {
    if (!has_blank) {
      char smallest = CHAR_MAX;
      for (int i = 0; i < init_size; ++i) {
        if (init_letters[i] < smallest) {
          smallest = init_letters[i];
          first_player_index = i;
          first_player_name = game_.players()[i].name();
        }
      }
      GUI::RenderTextCenteredX(
          renderer, gui_->jersey48(),
          first_player_name + " has the closest tile to 'A'",
          gui_->window_width() / 2, y[3] + (gap * 2), {255, 255, 255, 255});
      std::string text = first_player_name + " is the first player to move";
      GUI::RenderTextCenteredX(renderer, gui_->jersey48(), text,
                               gui_->window_width() / 2, y[3] + (gap * 4),
                               {255, 255, 255, 255});
    } else {
      GUI::RenderTextCenteredX(renderer, gui_->jersey48(), "Blank tile found!",
                               gui_->window_width() / 2, y[3] + (gap * 2),
                               {255, 255, 255, 255});
      std::string text = first_player_name + " is the first player to move";
      GUI::RenderTextCenteredX(renderer, gui_->jersey48(), text,
                               gui_->window_width() / 2, y[3] + (gap * 4),
                               {255, 255, 255, 255});
    }
  }
  if (SDL_GetTicks() - first_player_popup_start_time_ >
      static_cast<uint32_t>((init_size + 3) * 1000)) {
    game_.SetFirstPlayer(first_player_index);
    showing_first_player_popup_ = false;
    SDL_RenderClear(renderer);
  }
}

void IngameState::InitPlayerDecks() { game_.InitPlayerDecks(); }

void IngameState::UpdateBoardSize() {
  const int offset_x = gui_->window_width() / 40;
  const int offset_y = gui_->window_height() / 30;

  const int gap_x = gui_->window_width() / 400;
  const int gap_y = gui_->window_width() / 400;

  const int square_w =
      (gui_->window_width() / 2 - offset_x * 2 - gap_x * 14) / 15;
  const int square_h =
      (gui_->window_height() * 3 / 4 - offset_y * 2 - gap_y * 14) / 15;

  for (int row = 0; row < kBoardSize; ++row) {
    for (int col = 0; col < kBoardSize; ++col) {
      int x = offset_x + (col * (square_w + gap_x));
      int y = offset_y + (row * (square_h + gap_y));

      board_display_grid_.at(row).at(col) = {x, y, square_w, square_h};
    }
  }
}

void IngameState::RenderBoard(SDL_Renderer* renderer) {
  const auto board_display_format = game_.board().GetDisplayFormat();
  for (int row = 0; row < kBoardSize; ++row) {
    for (int col = 0; col < kBoardSize; ++col) {
      if (board_display_format.at(row).at(col) == ".") {
        gui_->RenderImage(renderer, "assets/textures/board/square_normal.png",
                          board_display_grid_.at(row).at(col));
      } else if (board_display_format.at(row).at(col) == "*") {
        gui_->RenderImage(renderer, "assets/textures/board/square_start.png",
                          board_display_grid_.at(row).at(col));
      } else if (board_display_format.at(row).at(col) == "DL") {
        gui_->RenderImage(renderer, "assets/textures/board/square_dl.png",
                          board_display_grid_.at(row).at(col));
      } else if (board_display_format.at(row).at(col) == "DW") {
        gui_->RenderImage(renderer, "assets/textures/board/square_dw.png",
                          board_display_grid_.at(row).at(col));
      } else if (board_display_format.at(row).at(col) == "TL") {
        gui_->RenderImage(renderer, "assets/textures/board/square_tl.png",
                          board_display_grid_.at(row).at(col));
      } else if (board_display_format.at(row).at(col) == "TW") {
        gui_->RenderImage(renderer, "assets/textures/board/square_tw.png",
                          board_display_grid_.at(row).at(col));
      } else {
        board_occupied.at(row).at(col) = true;
        std::string image_path = "assets/textures/board/";
        std::string letter = board_display_format.at(row).at(col);
        std::transform(letter.begin(), letter.end(), letter.begin(), tolower);
        image_path += letter + ".png";
        gui_->RenderImage(renderer, image_path,
                          board_display_grid_.at(row).at(col));
      }
    }
  }

  // Display tiles in pending placements
  const auto& deck = game_.current_player().deck();
  for (const auto& [tile_index, use, row, col] : pending_move_) {
    std::string image_path = "assets/textures/board/";
    std::string letter = std::string{deck.at(tile_index).letter()};

    if (deck.at(tile_index).IsBlankTile()) {
      letter = use;
    }

    std::transform(letter.begin(), letter.end(), letter.begin(), tolower);
    image_path += letter + ".png";

    std::string tile_bg_path;
    if (ValidateMove()) {
      tile_bg_path = "assets/textures/ingame/board_tile_bg_valid.png";
    } else {
      tile_bg_path = "assets/textures/ingame/board_tile_bg_invalid.png";
    }
    gui_->RenderImage(renderer, tile_bg_path,
                      {board_display_grid_.at(row).at(col).x - 3,
                       board_display_grid_.at(row).at(col).y - 3,
                       board_display_grid_.at(row).at(col).w + 6,
                       board_display_grid_.at(row).at(col).h + 6});
    gui_->RenderImage(renderer, image_path,
                      board_display_grid_.at(row).at(col));
  }
}

void IngameState::UpdateDeckSize() {
  const int offset_x = gui_->window_width() / 40;
  const int gap = gui_->window_width() / 60;
  const int tile_w = (board_display_grid_.at(0).back().x +
                      board_display_grid_.at(0).back().w - offset_x - 6 * gap) /
                     7;
  const int tile_h =
      (gui_->window_height() - (board_display_grid_.at(14).back().y +
                                board_display_grid_.at(14).back().h)) /
      4;
  const int y = board_display_grid_.at(14).at(0).y +
                (board_display_grid_.at(6).at(0).h * 2);

  for (int i = 0; i < 7; ++i) {
    int x = offset_x + (i * (tile_w + gap));
    deck_display_.at(i) = {x, y, tile_w, tile_h};
  }
  for (const auto& [j, use, row, col] : pending_move_) {
    deck_display_.at(j).h = 0;
    deck_display_.at(j).w = 0;
  }
}

void IngameState::RenderDeck(SDL_Renderer* renderer) {
  const auto deck = game_.current_player().deck();
  const int deck_size = static_cast<int>(deck.size());
  for (int i = 0; i < deck_size; ++i) {
    if (dragged_tile_index_ == i || deck.at(i).IsEmpty()) {
      continue;
    }
    if (deck.at(i).letter() == '?') {
      gui_->RenderImage(renderer, "assets/textures/board/blank.png",
                        deck_display_.at(i));
    } else {
      std::string image_path = "assets/textures/board/";
      std::string letter = std::string{deck.at(i).letter()};
      std::transform(letter.begin(), letter.end(), letter.begin(), tolower);
      image_path += letter + ".png";
      gui_->RenderImage(renderer, image_path, deck_display_.at(i));
    }
  }
}

void IngameState::UpdatePlayerinfoBoxes() {
  const int box_w = gui_->window_width() / 6;
  const int box_h = gui_->window_height() / 10;

  int x = gui_->window_width() / 2;
  const int offset_y = gui_->window_width() / 40;
  const int gap = gui_->window_height() / 40;

  for (int i = 0; i < static_cast<int>(player_infos_.size()); ++i) {
    int y = offset_y + (i * (box_h + gap));
    player_infos_[i].box = {x, y, box_w, box_h};
  }
}

void IngameState::RenderPlayerinfoBoxes(SDL_Renderer* renderer) {
  for (int i = 0; i < static_cast<int>(player_infos_.size()); ++i) {
    player_infos_[i].score = game_.players()[i].score();
    const auto& name = player_infos_[i].name;
    const auto& box = player_infos_[i].box;
    const std::string score =
        "Score: " + std::to_string(player_infos_[i].score);

    const int padding_x = box.w / 30;
    const int padding_y = box.h / 20;
    const int x = box.x + padding_x;
    const int h = (box.h - padding_y * 3) / 2;

    const int name_y = box.y + padding_y;
    const int score_y = name_y + h + padding_y;

    if (name == game_.current_player().name()) {
      gui_->RenderImage(
          renderer, "assets/textures/ingame/playerinfo_box_current.png", box);
    } else {
      gui_->RenderImage(renderer, "assets/textures/ingame/playerinfo_box.png",
                        box);
    }
    constexpr SDL_Color kWhite = {255, 255, 255, 255};
    GUI::RenderFixedHeightText(renderer, name, gui_->jersey32(), x, name_y, h,
                               kWhite);
    GUI::RenderFixedHeightText(renderer, score, gui_->jersey32(), x, score_y, h,
                               kWhite);
  }
}

void IngameState::UpdateMoveHistorySize() {
  const int box_w = gui_->window_width() / 20 * 9;
  const int box_x = player_infos_.back().box.x;
  const int box_y =
      player_infos_.back().box.y + (player_infos_.back().box.h / 3 * 4);
  const int box_h = (gui_->window_height() - box_y) / 10 * 9;
  movehistory_box_ = {box_x, box_y, box_w, box_h};

  const int padding_x = box_w * 1 / 36;
  const int arrow_h = box_h / 10;
  const int arrow_w = box_w / 20;
  const int arrow_y = box_y + (box_h / 4) - arrow_h;
  const int left_arrow_x = box_x + padding_x;
  const int right_arrow_x = left_arrow_x + arrow_w + (box_w / 12);
  history_left_arrow_ = {left_arrow_x, arrow_y, arrow_w, arrow_h};
  history_right_arrow_ = {right_arrow_x, arrow_y, arrow_w, arrow_h};

  const int gap = box_h * 1 / 10;
  const int entry_w = box_w - (padding_x * 2);
  const int entry1_y = box_y + (box_h * 3 / 10);
  const int entry_h = (box_h * 3 / 4 - gap * 2) / 2;
  const int entry_x = left_arrow_x;
  const int entry2_y = entry1_y + entry_h + gap;

  int entry1_index = visible_entries_indices_[0];
  if (entry1_index != -1) {
    movehistory_[entry1_index].box = {entry_x, entry1_y, entry_w, entry_h};
  }
  int entry2_index = visible_entries_indices_[1];
  if (entry2_index != -1) {
    movehistory_[entry2_index].box = {entry_x, entry2_y, entry_w, entry_h};
  }
}

void IngameState::RenderMoveHistory(SDL_Renderer* renderer) {
  UpdateMoveHistorySize();
  constexpr SDL_Color kWhite = {255, 255, 255, 255};
  gui_->RenderImage(renderer, "assets/textures/ingame/movehistory_box.png",
                    movehistory_box_);
  GUI::RenderText(renderer, "Move history", gui_->jersey48(),
                  history_left_arrow_.x, movehistory_box_.y, kWhite);

  gui_->RenderImage(renderer, "assets/textures/ingame/left_arrow.png",
                    history_left_arrow_);
  gui_->RenderImage(renderer, "assets/textures/ingame/right_arrow.png",
                    history_right_arrow_);

  constexpr SDL_Color kDLColor = {104, 162, 195, 255};
  constexpr SDL_Color kDWColor = {228, 162, 163, 255};
  constexpr SDL_Color kTLColor = {12, 103, 156, 255};
  constexpr SDL_Color kTWColor = {191, 78, 78, 255};

  for (int i = 0; i < 2; ++i) {
    const int index = visible_entries_indices_.at(i);
    const std::string entry_box_texture =
        "assets/textures/ingame/movehistory_entry_" + std::to_string(i + 1) +
        ".png";
    if (index != -1) {
      const auto& box = movehistory_[index].box;
      const int padding = box.h / 12;
      gui_->RenderImage(renderer, entry_box_texture, box);
      const auto& content = movehistory_[index].move_content;
      GUI::RenderFixedHeightText(renderer, content, gui_->jersey32(),
                                 box.x + padding, box.y + padding,
                                 box.h * 5 / 12, kWhite);
      if (movehistory_[index].move_type == MoveType::SUBMIT) {
        const int letter_h = box.h * 5 / 12;
        std::string words_str = "Words: ";
        int words_str_w = 0;
        GUI::RenderFixedHeightTextReturnWidth(
            renderer, words_str, gui_->jersey32(), box.x + padding,
            box.y + (box.h / 2), letter_h, kWhite, words_str_w);
        const auto words = movehistory_[index].words;
        int x = box.x + padding + words_str_w;
        for (const auto& word : words) {
          int letter_gap = 1;
          for (const auto& [letter, multiplier] : word.content()) {
            std::string letter_str;
            letter_str += static_cast<char>(toupper(letter));
            SDL_Color color;
            switch (multiplier) {
              case core::Square::Multiplier::kNormal: {
                color = kWhite;
                break;
              }
              case core::Square::Multiplier::kDoubleLetter: {
                color = kDLColor;
                break;
              }
              case core::Square::Multiplier::kDoubleWord: {
                color = kDWColor;
                break;
              }
              case core::Square::Multiplier::kTripleLetter: {
                color = kTLColor;
                break;
              }
              case core::Square::Multiplier::kTripleWord: {
                color = kTWColor;
                break;
              }
            }
            int letter_w = 0;
            GUI::RenderFixedHeightTextReturnWidth(
                renderer, letter_str, gui_->jersey32(), x, box.y + (box.h / 2),
                letter_h, color, letter_w);
            x += letter_w + letter_gap;
          }
          x += letter_gap * 8;
        }
      }
    }
  }
}

void IngameState::HandleMoveHistoryNavigation(SDL_Event& event) {
  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    SDL_Point mouse_pos = MousePos();
    // See latest moves, indices increase
    if (InRect(mouse_pos, history_left_arrow_)) {
      const int i1 = visible_entries_indices_[0];
      const int mh_size = static_cast<int>(movehistory_.size());
      if (i1 + 1 < mh_size) {
        visible_entries_indices_[1] += 2;
        if (i1 + 2 > mh_size - 1) {
          visible_entries_indices_[0] += 1;
        } else {
          visible_entries_indices_[0] += 2;
        }
      }
    }
    // See previous moves, indices decrease
    if (InRect(mouse_pos, history_right_arrow_)) {
      history_view_locked = true;
      int& i1 = visible_entries_indices_[0];
      int& i2 = visible_entries_indices_[1];
      if (i1 - 2 >= 0) {
        if (i2 == 1) {
          i1 -= 1;
          i2 = 0;
        } else {
          i1 -= 2;
          i2 -= 2;
        }
      }
    }
  }
}

void IngameState::UpdateActionButtonsSize() {
  const int deck_bottom_edge =
      board_display_grid_.at(14).at(0).y +
      (board_display_grid_.at(6).at(0).h * 2) +
      ((gui_->window_height() - (board_display_grid_.at(14).back().y +
                                 board_display_grid_.at(14).back().h)) /
       4);

  const int x = gui_->window_width() / 40;
  const int gap = gui_->window_width() / 60;
  const int y = deck_bottom_edge + gap;

  const int w = (board_display_grid_[0][14].x + board_display_grid_[0][14].h -
                 x - 2 * gap) /
                3;
  const int h = (gui_->window_height() - y) / 2;

  pass_button_ = {x, y, w, h};
  swap_button_ = {x + w + gap, y, w, h};
  submit_button_ = {x + (2 * (w + gap)), y, w, h};
}

void IngameState::RenderActionButtons(SDL_Renderer* renderer) {
  gui_->RenderImage(renderer, "assets/textures/ingame/button_pass.png",
                    pass_button_);
  gui_->RenderImage(renderer, "assets/textures/ingame/button_swap.png",
                    swap_button_);
  gui_->RenderImage(renderer, "assets/textures/ingame/button_submit.png",
                    submit_button_);
  if (!submit_error_message.empty()) {
    GUI::RenderTextCenteredX(renderer, gui_->jersey32(), submit_error_message,
                             board_display_grid_[7][7].x,
                             submit_button_.y + submit_button_.h + 20,
                             {255, 0, 0, 255});
  }
}

void IngameState::RenderDraggedTile(SDL_Renderer* renderer) {
  if (dragging_tile_) {
    char dragged_tile_letter =
        game_.current_player().deck().at(dragged_tile_index_).letter();
    std::string image_path = "assets/textures/board/";
    std::string letter = (dragged_tile_letter == '?')
                             ? "blank"
                             : std::string{dragged_tile_letter};
    std::transform(letter.begin(), letter.end(), letter.begin(), tolower);
    image_path += letter + ".png";
    gui_->RenderImage(renderer, image_path, dragged_tile_rect_);
  }
}

void IngameState::RenderSwapPopup(SDL_Renderer* renderer) {
  if (!show_swap_popup_) {
    return;
  }

  // Modal box
  int modal_w = gui_->window_width() / 2;
  int modal_h = gui_->window_height() / 2;
  swap_popup_box_ = {(gui_->window_width() - modal_w) / 2,
                     (gui_->window_height() - modal_h) / 2, modal_w, modal_h};

  gui_->RenderImage(renderer,
                    "assets/textures/ingame/swap_popup_background.png",
                    swap_popup_box_);

  // Confirm button
  const int button_w = modal_w / 5;
  const int button_h = modal_h / 7;
  const int button_x = swap_popup_box_.x + ((modal_w - button_w) / 2);

  const int confirm_button_y = swap_popup_box_.y + (modal_h / 4);
  swap_confirm_button_ = {button_x, confirm_button_y, button_w, button_h};
  gui_->RenderImage(renderer, "assets/textures/ingame/button_confirm.png",
                    swap_confirm_button_);

  // Cancel button
  const int cancel_button_y = confirm_button_y + button_h + (modal_h / 30);
  swap_cancel_button_ = {button_x, cancel_button_y, button_w, button_h};
  gui_->RenderImage(renderer, "assets/textures/ingame/button_cancel.png",
                    swap_cancel_button_);

  // Deck tiles
  const auto& deck = game_.current_player().deck();
  const int text_padding = modal_w / 20;
  int tile_h = modal_w / 10;
  int start_x = swap_popup_box_.x + text_padding;
  int y = cancel_button_y + button_h +
          ((swap_popup_box_.y + modal_h - (cancel_button_y + button_h)) / 5);
  int gap = modal_w / 50;
  int tile_w = (modal_w - (2 * text_padding + 6 * gap)) / 7;

  if (no_tile_selected_error) {
    const int error_w = button_w * 2;
    const int error_h = button_h / 2;
    const int error_x = swap_popup_box_.x + ((modal_w - error_w) / 2);
    const int error_y = cancel_button_y + button_h + (modal_h / 40);
    y += error_h + modal_h / 40;
    gui_->RenderImage(renderer,
                      "assets/textures/ingame/swap_error_no_tile_selected.png",
                      {error_x, error_y, error_w, error_h});
  }

  if (not_enough_tiles_in_bag_error) {
    const int error_h = button_h / 2;
    const int error_x = gui_->window_width() / 2;
    const int error_y = cancel_button_y + button_h + (modal_h / 40);
    y += error_h + modal_h / 40;
    int width = 0;
    GUI::RenderFixedHeightCenteredText(
        renderer, "Not enough tiles in bag to swap", gui_->jersey32(), error_x,
        error_y, error_h, {255, 0, 0, 255}, width);
  }

  for (int i = 0; i < static_cast<int>(deck.size()); ++i) {
    swap_deck_.at(i) = {start_x + (i * (tile_w + gap)), y, tile_w, tile_h};

    bool selected =
        std::find(selected_swap_indices_.begin(), selected_swap_indices_.end(),
                  i) != selected_swap_indices_.end();
    if (selected) {
      SDL_Rect outline = {swap_deck_.at(i).x - 2, swap_deck_.at(i).y - 2,
                          swap_deck_.at(i).w + 4, swap_deck_.at(i).h + 4};
      gui_->RenderImage(renderer,
                        "assets/textures/ingame/selected_tile_background.png",
                        outline);
    }
    if (game_.current_player().deck().at(i).IsEmpty()) {
      continue;
    }
    std::string image_path = "assets/textures/board/";
    std::string letter = (deck.at(i).letter() == '?')
                             ? "blank"
                             : std::string{deck.at(i).letter()};
    std::transform(letter.begin(), letter.end(), letter.begin(), tolower);
    image_path += letter + ".png";
    gui_->RenderImage(renderer, image_path, swap_deck_.at(i));
  }
}

void IngameState::RenderBlankSelectPopup(SDL_Renderer* renderer) {
  if (!show_select_for_blank_popup) {
    return;
  }

  const int modal_w = gui_->window_width() / 2;
  const int modal_h = gui_->window_height() / 11 * 10;
  select_for_blank_box_ = {(gui_->window_width() - modal_w) / 2,
                           (gui_->window_height() - modal_h) / 2, modal_w,
                           modal_h};

  gui_->RenderImage(
      renderer, "assets/textures/ingame/select_tile_for_blank_background.png",
      select_for_blank_box_);

  const int padding = modal_w / 20;
  const int start_x = select_for_blank_box_.x + padding;
  const int gap_x = modal_w / 40;
  const int gap_y = modal_h / 40;
  const int letter_w = (modal_w - 2 * padding - 5 * gap_x) / 6;
  const int letter_h = ((modal_h * 7 / 9) - padding - 4 * gap_y) / 5;
  const int y1 = modal_h * 5 / 18;
  const int y2 = y1 + letter_h + gap_y;
  const int y3 = y2 + letter_h + gap_y;
  const int y4 = y3 + letter_h + gap_y;
  const int y5 = y4 + letter_h + gap_y;
  for (auto& rect : letter_rects_) {
    rect.w = letter_w;
    rect.h = letter_h;
  }

  const std::array<int, 5> letters_per_row{6, 6, 6, 6, 2};
  const std::array<int, 5> row_y{y1, y2, y3, y4, y5};

  int letter_index = 0;
  for (int row = 0; row < 5; ++row) {
    for (int col = 0; col < letters_per_row.at(row); ++col) {
      int x = 0;
      if (row < 4) {
        x = start_x + col * (gap_x + letter_w);
      } else {
        if (col == 0) {
          x = letter_rects_.at(2).x;
        } else if (col == 1) {
          x = letter_rects_.at(3).x;
        }
      }

      letter_rects_.at(letter_index).x = x;
      letter_rects_.at(letter_index).y = row_y.at(row);

      bool selected = (selected_letter_index_ == letter_index);
      if (selected) {
        SDL_Rect outline = {letter_rects_.at(letter_index).x - 2,
                            letter_rects_.at(letter_index).y - 2,
                            letter_rects_.at(letter_index).w + 4,
                            letter_rects_.at(letter_index).h + 4};
        gui_->RenderImage(renderer,
                          "assets/textures/ingame/selected_tile_background.png",
                          outline);
      }

      ++letter_index;
    }
  }

  for (char c = 'a'; c <= 'z'; ++c) {
    std::string image_path = "assets/textures/board/" + std::string{c} + ".png";
    gui_->RenderImage(renderer, image_path, letter_rects_.at(c - 'a'));
  }
}

void IngameState::RenderEndgamePopup(SDL_Renderer* renderer) {
  if (!game_over) {
    return;
  }

  int box_w = gui_->window_width() * 13 / 20;
  int box_h = gui_->window_height() * 4 / 5;
  endgame_popup_box_ = {(gui_->window_width() - box_w) / 2,
                        (gui_->window_height() - box_h) / 2, box_w, box_h};
  gui_->RenderImage(renderer, "assets/textures/ingame/endgame_popup_box.png",
                    endgame_popup_box_);

  const int padding_x = box_w / 20;
  const int padding_y = box_h / 20;

  const int button_w = box_w * 3 / 10;
  const int button_h = box_h * 2 / 15;
  const int button_x = endgame_popup_box_.x + ((box_w - button_w) / 2);
  const int button_y = endgame_popup_box_.y + (box_h - padding_y - button_h);
  back_button_ = {button_x, button_y, button_w, button_h};
  gui_->RenderImage(renderer, "assets/textures/ingame/button_back.png",
                    back_button_);

  constexpr SDL_Color kWhite = {255, 255, 255, 255};
  const int winner_text_y = endgame_popup_box_.y + padding_y;
  GUI::RenderTextCenteredX(renderer, gui_->jersey64(),
                           game_.winner().name() + " wins!",
                           gui_->window_width() / 2, winner_text_y, kWhite);

  for (int i = 0; i < static_cast<int>(endgame_info_.size()); ++i) {
    GUI::RenderText(renderer, endgame_info_[i].name, gui_->jersey48(),
                    endgame_popup_box_.x + padding_x,
                    winner_text_y + (padding_y * 2) + (box_h / 6 * i), kWhite);
    GUI::RenderText(
        renderer, endgame_info_[i].content(), gui_->jersey48(),
        endgame_popup_box_.x + padding_x,
        winner_text_y + (padding_y * 2) + (box_h / 6 * i) + (padding_y * 3 / 2),
        kWhite);
  }
}

void IngameState::Render(SDL_Renderer* renderer) {
  UpdateBoardSize();
  RenderBoard(renderer);
  UpdateDeckSize();
  RenderDeck(renderer);
  UpdatePlayerinfoBoxes();
  RenderPlayerinfoBoxes(renderer);
  UpdateActionButtonsSize();
  RenderActionButtons(renderer);
  RenderMoveHistory(renderer);

  RenderDraggedTile(renderer);

  RenderSwapPopup(renderer);
  RenderBlankSelectPopup(renderer);
  RenderEndgamePopup(renderer);
  RenderFirstPlayerInitPopup(renderer);
}

bool IngameState::ValidateMove() {
  auto game_temp = game_;
  const auto response = game_temp.ExecutePlaceMove(pending_move_);

  return response.status == core::Board::ResponseStatus::kSuccess;
}

void IngameState::SubmitMove() {
  const std::string name = game_.current_player().name();
  const auto response = game_.ExecutePlaceMove(pending_move_);

  if (response.status != core::Board::ResponseStatus::kSuccess) {
    switch (response.status) {
      case core::Board::ResponseStatus::kNotInStartingSquare: {
        submit_error_message = "Your tiles must cover the starting square";
        break;
      }
      case core::Board::ResponseStatus::kNotAligned: {
        submit_error_message = "Your tiles must be in a straight line";
        break;
      }
      case core::Board::ResponseStatus::kNotAdjacent: {
        submit_error_message =
            "Your tiles must be continuous and connected to existing tiles";
        break;
      }
      case core::Board::ResponseStatus::kWordsInvalid: {
        submit_error_message = "Invalid words: " + response.invalid_word;
        break;
      }
      default: {
        break;
      }
    }
  } else {
    const std::string turn_number = std::to_string(game_.turn_number());
    const std::string points = std::to_string(response.move_points);
    const std::string message =
        turn_number + ". " + name + " scored " + points + " points";
    movehistory_.push_back(
        {{}, name, MoveType::SUBMIT, message, response.words});
    pending_move_.clear();
  }
}

void IngameState::HandleTileDrag(SDL_Event& event) {
  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    SDL_Point mouse_pos = MousePos();
    // Check if clicked a tile in deck
    const auto& deck = game_.current_player().deck();
    for (int i = 0; i < static_cast<int>(deck.size()); ++i) {
      if (InRect(mouse_pos, deck_display_.at(i))) {
        submit_error_message.clear();
        dragging_tile_ = true;
        dragged_tile_index_ = i;
        drag_offset_.x = mouse_pos.x - deck_display_.at(i).x;
        drag_offset_.y = mouse_pos.y - deck_display_.at(i).y;
        dragged_tile_rect_ = deck_display_.at(i);
        break;
      }
    }
  }
  // Update drag position
  else if (event.type == SDL_MOUSEMOTION && dragging_tile_) {
    dragged_tile_rect_.x = event.motion.x - drag_offset_.x;
    dragged_tile_rect_.y = event.motion.y - drag_offset_.y;
  }
  // Drop tile
  else if (event.type == SDL_MOUSEBUTTONUP &&
           event.button.button == SDL_BUTTON_LEFT && dragging_tile_) {
    SDL_Point mouse_pos = MousePos();

    // Check if dropped on a board square
    bool placed = false;
    for (int row = 0; row < kBoardSize && !placed; ++row) {
      for (int col = 0; col < kBoardSize && !placed; ++col) {
        if (InRect(mouse_pos, board_display_grid_.at(row).at(col)) &&
            !board_occupied.at(row).at(col)) {
          if (!game_.current_player()
                   .deck()
                   .at(dragged_tile_index_)
                   .IsBlankTile()) {
            board_occupied.at(row).at(col) = true;
            pending_move_.push_back({dragged_tile_index_, 0, row, col});
            placed = true;
            SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_HAND));
          } else {
            board_occupied.at(row).at(col) = true;
            placed = true;
            blank_tile_index_ = dragged_tile_index_;
            blank_tile_row_ = row;
            blank_tile_col_ = col;
            show_select_for_blank_popup = true;
            selected_letter_index_ = -1;
          }
        }
      }
    }

    // Reset drag state
    dragging_tile_ = false;
    dragged_tile_index_ = -1;
  }
}

void IngameState::HandleBoardSquareClick(SDL_Event& event) {
  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    SDL_Point mouse_pos = MousePos();
    for (int row = 0; row < 15; ++row) {
      for (int col = 0; col < 15; ++col) {
        SDL_Rect square = board_display_grid_.at(row).at(col);
        bool square_in_pending_placements = false;
        auto found = pending_move_.end();
        for (auto it = pending_move_.begin(); it != pending_move_.end(); ++it) {
          if (it->row == row && it->col == col) {
            square_in_pending_placements = true;
            found = it;
            break;
          }
        }
        bool hovering_square = (InRect(mouse_pos, square));
        if (hovering_square && found != pending_move_.end() &&
            square_in_pending_placements) {
          submit_error_message.clear();
          pending_move_.erase(found);
          board_occupied.at(row).at(col) = false;
        }
      }
    }
  }
}

void IngameState::HandleActionButtons(SDL_Event& event) {
  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    SDL_Point mouse_pos = MousePos();
    if (InRect(mouse_pos, pass_button_)) {
      ClearPendingPlayerMove();
      const std::string name = game_.current_player().name();
      game_.ExecutePassMove();
      const std::string turn_number = std::to_string(game_.turn_number());
      const std::string message =
          turn_number + ". " + name + " passed their turn";
      movehistory_.push_back(
          {{}, name, IngameState::MoveType::PASS, message, {}});
    }
    if (InRect(mouse_pos, swap_button_)) {
      not_enough_tiles_in_bag_error = false;
      no_tile_selected_error = false;
      ClearPendingPlayerMove();
      submit_error_message.clear();
      show_swap_popup_ = true;
      selected_swap_indices_.clear();
    }
    if (InRect(mouse_pos, submit_button_)) {
      submit_error_message.clear();
      if (!pending_move_.empty()) {
        SubmitMove();
      } else {
        submit_error_message = "Place a tile before submitting";
      }
    }
  }
}

void IngameState::HandleSwapPopupEvent(SDL_Event& event) {
  if (!show_swap_popup_) {
    return;
  }

  SDL_Point mouse_pos = MousePos();

  auto over_rect = [&](const SDL_Rect& r) { return InRect(mouse_pos, r); };
  auto over_any_tile = [&] {
    for (int i = 0; i < static_cast<int>(swap_deck_.size()); ++i) {
      if (over_rect(swap_deck_.at(i))) {
        return true;
      }
    }
    return false;
  };

  const bool hovering = over_any_tile() || over_rect(swap_confirm_button_) ||
                        over_rect(swap_cancel_button_);
  SDL_SetCursor(gui_->cursor(hovering ? SDL_SYSTEM_CURSOR_HAND
                                      : SDL_SYSTEM_CURSOR_ARROW));

  if (event.type != SDL_MOUSEBUTTONDOWN ||
      event.button.button != SDL_BUTTON_LEFT) {
    return;
  }

  for (int i = 0; i < static_cast<int>(swap_deck_.size()); ++i) {
    if (over_rect(swap_deck_.at(i))) {
      no_tile_selected_error = false;
      auto it = std::find(selected_swap_indices_.begin(),
                          selected_swap_indices_.end(), i);
      if (it == selected_swap_indices_.end()) {
        selected_swap_indices_.push_back(i);
      } else {
        selected_swap_indices_.erase(it);
      }
      break;
    }
  }

  if (over_rect(swap_confirm_button_)) {
    if (!selected_swap_indices_.empty()) {
      if (static_cast<int>(selected_swap_indices_.size()) >
          game_.bag().num_tiles_remanining()) {
        no_tile_selected_error = false;
        not_enough_tiles_in_bag_error = true;
        return;
      }

      const std::string name = game_.current_player().name();
      game_.ExecuteSwapMove(selected_swap_indices_);

      const std::string turn_number = std::to_string(game_.turn_number());
      const std::string message =
          turn_number + ". " + name + " swapped some tiles";
      movehistory_.push_back(
          {{}, name, IngameState::MoveType::SWAP, message, {}});

      ClearPendingPlayerMove();
      show_swap_popup_ = false;
    } else {
      not_enough_tiles_in_bag_error = false;
      no_tile_selected_error = true;
    }
  }

  if (over_rect(swap_cancel_button_)) {
    show_swap_popup_ = false;
    selected_swap_indices_.clear();
  }
}

void IngameState::HandleBlankSelectPopupEvent(SDL_Event& event) {
  if (!show_select_for_blank_popup) {
    return;
  }

  SDL_Point mouse_pos = MousePos();
  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    for (int i = 0; i < 26; ++i) {
      if (InRect(mouse_pos, letter_rects_.at(i))) {
        selected_letter_index_ = i;
        blank_tile_use_ = static_cast<char>(i + 'a');
        pending_move_.push_back({blank_tile_index_, blank_tile_use_,
                                 blank_tile_row_, blank_tile_col_});
        show_select_for_blank_popup = false;
      }
    }
  }
}

void IngameState::HandleEndgamePopupEvent(SDL_Event& event) {
  if (!game_over) {
    return;
  }

  SDL_Point mouse_pos = MousePos();
  if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN) {
    bool is_hovering_back = InRect(mouse_pos, back_button_);

    if (is_hovering_back) {
      SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_HAND));
      if (event.type == SDL_MOUSEBUTTONDOWN &&
          event.button.button == SDL_BUTTON_LEFT) {
        game_over = false;
        gui_->ChangeState(GUI::GameStateType::MainMenu);
      }
    } else {
      SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_ARROW));
    }
  }
}

void IngameState::HandleHovering() {
  SDL_Point mouse_pos = MousePos();

  bool hovering_placed_tile_on_board = false;
  for (int row = 0; row < 15; ++row) {
    if (hovering_placed_tile_on_board) {
      break;
    }
    for (int col = 0; col < 15; ++col) {
      SDL_Rect square = board_display_grid_.at(row).at(col);
      bool square_in_pending_placements = false;
      for (const auto& p : pending_move_) {
        if (p.row == row && p.col == col) {
          square_in_pending_placements = true;
          break;
        }
      }
      bool hovering_square = (InRect(mouse_pos, square));
      if (hovering_square && square_in_pending_placements) {
        hovering_placed_tile_on_board = true;
        break;
      }
    }
  }

  is_hovering_ =
      InRect(mouse_pos, pass_button_) || InRect(mouse_pos, swap_button_) ||
      InRect(mouse_pos, submit_button_) ||
      InRect(mouse_pos, history_left_arrow_) ||
      InRect(mouse_pos, history_right_arrow_) || hovering_placed_tile_on_board;

  if (is_hovering_) {
    SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_HAND));
  } else {
    SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_ARROW));
  }
}

void IngameState::HandleEvent(SDL_Event& event) {
  if (showing_first_player_popup_) {
    return;
  }
  if (show_swap_popup_) {
    HandleSwapPopupEvent(event);
    return;
  }
  if (show_select_for_blank_popup) {
    HandleBlankSelectPopupEvent(event);
    return;
  }
  if (game_over) {
    HandleEndgamePopupEvent(event);
    return;
  }
  switch (event.type) {
    case SDL_MOUSEMOTION:
      HandleHovering();
      HandleTileDrag(event);
      break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      HandleTileDrag(event);
      HandleBoardSquareClick(event);
      HandleActionButtons(event);
      HandleMoveHistoryNavigation(event);
      break;

    default:
      break;
  }
}

void IngameState::UpdateEndgameInfo() {
  for (int i = 0; i < game_.num_players(); ++i) {
    // Name and final score first
    std::string name = game_.players()[i].name();
    std::string final_score = std::to_string(game_.players()[i].score());

    // Top word and top word score
    std::string top_word;
    int top_word_points = 0;
    for (const auto& move : game_.move_history()) {
      if (move.type == game::Game::MoveType::kPlacing &&
          move.player_name == name) {
        for (const auto& word : move.words) {
          if (word.points() > top_word_points) {
            top_word_points = word.points();
            top_word = word.AsString();
          }
        }
      }
    }
    endgame_info_.emplace_back(name, final_score, top_word,
                               std::to_string(top_word_points));
  }
}

void IngameState::Update() {
  if (gui_->lexicon() != nullptr && game_.lexicon() == nullptr) {
    game_.SetLexicon(gui_->lexicon());
    if (game_.lexicon()->type() == core::Dictionary::CSW) {
      std::cout << "Using CSW lexicon\n";
    } else if (game_.lexicon()->type() == core::Dictionary::TWL) {
      std::cout << "Using TWL lexicon\n";
    }
  }

  if (game_.IsOver()) {
    if (!game_over) {
      game_over = true;
      UpdateEndgameInfo();
    }
  }
  if (!movehistory_.empty()) {
    if (visible_entries_indices_[0] ==
        static_cast<int>(movehistory_.size()) - 1) {
      history_view_locked = false;
    }
    if (movehistory_.size() == 1) {
      visible_entries_indices_ = {0, -1};
    } else if (movehistory_.size() >= 2) {
      if (!history_view_locked) {
        visible_entries_indices_[0] = static_cast<int>(movehistory_.size()) - 1;
        visible_entries_indices_[1] = static_cast<int>(movehistory_.size()) - 2;
      }
    }
  } else {
    visible_entries_indices_.fill(-1);
  }
}
}  // namespace gui
