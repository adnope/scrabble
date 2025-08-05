#include "ingame_state.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

#include "SDL_mouse.h"
#include "SDL_render.h"
#include "core/board.hpp"
#include "core/player.hpp"
#include "gui.hpp"

namespace gui {
IngameState::IngameState(GUI* gui, core::Dictionary::DictionaryType dict_type,
                         const std::vector<std::string>& player_names)
    : gui_(gui), game_(dict_type, player_names) {
  InitFirstPlayer();
  InitPlayerDecks();
  for (const auto& name : player_names) {
    player_infos_.push_back({{}, name, 0});
  }
}

void IngameState::ClearPendingPlayerMove() {
  for (const auto& [i, row, col] : player_move_) {
    board_occupied.at(row).at(col) = false;
  }
  player_move_.clear();
}

void IngameState::InitFirstPlayer() {
  constexpr int kIntMax = 9999;
  int first_player_index = 0;
  int distance_from_a = kIntMax;
  auto game_bag = game_.bag();
  auto game_players = game_.players();
  for (int i = 0; i < game_.num_players(); ++i) {
    const char letter = game_bag.DrawTile().letter();
    std::cout << game_players[i].name() << ": " << letter << '\n';

    if (letter == '?') {
      std::cout << game_players[i].name() << " has a blank tile.\n";
      first_player_index = i;
      break;
    }

    if (letter - 'A' < distance_from_a) {
      first_player_index = i;
      distance_from_a = letter - 'A';
    }
  }
  std::cout << game_players[first_player_index].name() << " moves first!\n\n";
  game_.SetFirstPlayer(first_player_index);
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
  for (const auto& [tile_index, row, col] : player_move_) {
    std::string image_path = "assets/textures/board/";
    std::string letter = std::string{deck.at(tile_index).letter()};

    if (letter == "?") {
      letter = "blank";
    }

    std::transform(letter.begin(), letter.end(), letter.begin(), tolower);
    image_path += letter + ".png";
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
  for (const auto& [j, row, col] : player_move_) {
    deck_display_.at(j).h = 0;
    deck_display_.at(j).w = 0;
  }
}

void IngameState::RenderDeck(SDL_Renderer* renderer) {
  const auto deck = game_.current_player().deck();
  const int deck_size = static_cast<int>(deck.size());
  for (int i = 0; i < deck_size; ++i) {
    if (dragged_tile_index_ == i) {
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

void IngameState::UpdatePlayerinfoBoxesSize() {
  const int box_w = gui_->window_width() / 4;
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

    const int padding = box.w / 30;
    const int x = box.x + padding;
    const int name_y = box.y + padding;

    const int gap = box.h / 3;
    const int score_y = name_y + gap;

    constexpr SDL_Color kWhite = {255, 255, 255, 255};
    if (name == game_.current_player().name()) {
      gui_->RenderImage(
          renderer, "assets/textures/ingame/playerinfo_box_current.png", box);
    } else {
      gui_->RenderImage(renderer, "assets/textures/ingame/playerinfo_box.png",
                        box);
    }
    GUI::RenderText(renderer, name, gui_->jersey32(), x, name_y, kWhite);
    GUI::RenderText(renderer, score, gui_->jersey32(), x, score_y, kWhite);
  }
}

void IngameState::UpdateMoveHistorySize() {
  const int box_w = gui_->window_width() / 20 * 9;
  const int box_x = player_infos_.back().box.x;
  const int box_y =
      player_infos_.back().box.y + (player_infos_.back().box.h / 3 * 4);
  const int box_h = (gui_->window_height() - box_y) / 10 * 9;
  movehistory_box_ = {box_x, box_y, box_w, box_h};
}

void IngameState::RenderMoveHistory(SDL_Renderer* renderer) {
  constexpr SDL_Color kWhite = {255, 255, 255, 255};
  const int boxlabel_padding = movehistory_box_.h / 40;
  gui_->RenderImage(renderer, "assets/textures/ingame/movehistory_box.png",
                    movehistory_box_);
  GUI::RenderText(renderer, "Move history", gui_->jersey32(),
                  movehistory_box_.x + boxlabel_padding,
                  movehistory_box_.y + boxlabel_padding, kWhite);
}

void IngameState::UpdateActionButtonsSize() {
  // const int y = board_display_grid_.at(14).at(0).y +
  //               (board_display_grid_.at(6).at(0).h * 2);
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

void IngameState::Render(SDL_Renderer* renderer) {
  UpdateBoardSize();
  RenderBoard(renderer);
  UpdateDeckSize();
  RenderDeck(renderer);
  UpdatePlayerinfoBoxesSize();
  RenderPlayerinfoBoxes(renderer);
  UpdateActionButtonsSize();
  RenderActionButtons(renderer);
  UpdateMoveHistorySize();
  RenderMoveHistory(renderer);

  RenderDraggedTile(renderer);
}

void IngameState::SubmitMove() {
  const auto response = game_.ExecutePlaceMove(player_move_);

  if (response.status != core::Board::ResponseStatus::kSuccess) {
    std::cout << "Invalid move!\n";
  } else {
    std::cout << response.move_points << "points\n";
    std::cout << "words: ";
    for (const auto& word : response.words) {
      std::cout << word.AsString() << " ";
    }
    std::cout << '\n';
    player_move_.clear();
  }
}

void IngameState::HandleTileDrag(SDL_Event& event) {
  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    SDL_Point mouse_pos{event.button.x, event.button.y};

    // Check if clicked a tile in deck
    const auto& deck = game_.current_player().deck();
    for (int i = 0; i < static_cast<int>(deck.size()); ++i) {
      if (SDL_PointInRect(&mouse_pos, &deck_display_.at(i)) == 1) {
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
    SDL_Point mouse_pos{event.button.x, event.button.y};

    // Check if dropped on a board square
    bool placed = false;
    for (int row = 0; row < kBoardSize && !placed; ++row) {
      for (int col = 0; col < kBoardSize && !placed; ++col) {
        if (SDL_PointInRect(&mouse_pos, &board_display_grid_.at(row).at(col)) ==
                1 &&
            !board_occupied.at(row).at(col)) {
          // Place tile in game logic
          std::cout << "Tile placed at " << row << " " << col << '\n';
          board_occupied.at(row).at(col) = true;
          player_move_.push_back({dragged_tile_index_, row, col});
          for (const auto& [tindex, prow, pcol] : player_move_) {
            std::cout << tindex << " " << prow << " " << pcol << '\n';
          }
          placed = true;
        }
      }
    }

    // Reset drag state
    dragging_tile_ = false;
    dragged_tile_index_ = -1;
  }
}

void IngameState::HandleBoardSquareClick(SDL_Event& event) {
  SDL_Point mouse_pos;
  if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN) {
    mouse_pos.x = event.motion.x;
    mouse_pos.y = event.motion.y;

    for (int row = 0; row < 15; ++row) {
      for (int col = 0; col < 15; ++col) {
        SDL_Rect square = board_display_grid_.at(row).at(col);
        bool square_in_pending_placements = false;
        auto found = player_move_.end();
        for (auto it = player_move_.begin(); it != player_move_.end(); ++it) {
          if (it->row == row && it->col == col) {
            square_in_pending_placements = true;
            found = it;
            break;
          }
        }
        bool hovering_square = (SDL_PointInRect(&mouse_pos, &square) == 1);

        if (hovering_square && square_in_pending_placements) {
          SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_HAND));
          if (event.type == SDL_MOUSEBUTTONDOWN &&
              event.button.button == SDL_BUTTON_LEFT &&
              found != player_move_.end()) {
            player_move_.erase(found);
            board_occupied.at(row).at(col) = false;
          }
        } else {
          SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_ARROW));
        }
      }
    }
  }
}

void IngameState::HandleActionButtons(SDL_Event& event) {
  SDL_Point mouse_pos;
  if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN) {
    mouse_pos.x = event.motion.x;
    mouse_pos.y = event.motion.y;

    bool is_hovering_pass = SDL_PointInRect(&mouse_pos, &pass_button_) == 1;
    bool is_hovering_swap = SDL_PointInRect(&mouse_pos, &swap_button_) == 1;
    bool is_hovering_submit = SDL_PointInRect(&mouse_pos, &submit_button_) == 1;

    if (is_hovering_pass || is_hovering_swap || is_hovering_submit) {
      SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_HAND));
    } else {
      SDL_SetCursor(gui_->cursor(SDL_SYSTEM_CURSOR_ARROW));
    }

    if ((is_hovering_pass || is_hovering_swap || is_hovering_submit) &&
        event.type == SDL_MOUSEBUTTONDOWN &&
        event.button.button == SDL_BUTTON_LEFT) {
      if (is_hovering_pass) {
        ClearPendingPlayerMove();
        game_.ExecutePassMove();
        std::cout << game_.current_player().name() << " passed their turn\n";
      }
      if (is_hovering_swap) {
      }
      if (is_hovering_submit) {
        SubmitMove();
      }
    }
  }
}

void IngameState::HandleEvent(SDL_Event& event) {
  HandleTileDrag(event);
  HandleBoardSquareClick(event);
  HandleActionButtons(event);
}

void IngameState::Update() {
  if (game_.IsOver()) {
    gui_->ChangeState(GUI::GameStateType::MainMenu);
  }
}
}  // namespace gui

// TODO (Duy Nguyen):
// Swap mechanism
// Blank tile mechanism
// Handle move validation exceptions
// Move history
// Input player names view
// Load dictionary when create game
// Create 'determining first player' view
// Add time constraint
// Fix settings menu