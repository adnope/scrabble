#include "bot.hpp"

#include <algorithm>
#include <string>

#include "bag.hpp"
#include "core/board.hpp"

namespace core {

Bot::Bot(const std::string& player_name, int score)
    : Player(player_name, score), lexicon_(std::make_unique<Lexicon>()) {
    // Cần một Lexicon hợp lệ, ví dụ:
    lexicon_->PreloadDictionary(core::Dictionary::DictionaryType::CSW);
}

// Hàm hỗ trợ: Kiểm tra xem có thể di chuyển từ vị trí (row, col) theo hướng và
// chiều
static bool moves_available(int row, int col, bool horizontal, bool plus) {
  if (row < 0 || row >= Board::kHeight || col < 0 || col >= Board::kWidth) {
    return false;
  }
  if (horizontal) {
    if (plus && col >= Board::kWidth - 1) {
      return false;
    }
    if (!plus && col <= 0) {
      return false;
    }
  } else {
    if (plus && row >= Board::kHeight - 1) {
      return false;
    }
    if (!plus && row <= 0) {
      return false;
    }
  }
  return true;
}

// Hàm hỗ trợ: Cập nhật vị trí (row, col) theo hướng và chiều
static void deplacement(bool horizontal, bool plus, int& row, int& col) {
  if (plus) {
    if (horizontal) {
      if (col < Board::kWidth - 1) {
        col++;
      }
    } else {
      if (row < Board::kHeight - 1) {
        row++;
      }
    }
  } else {
    if (horizontal) {
      if (col > 0) {
        col--;
      }
    } else {
      if (row > 0) {
        row--;
      }
    }
  }
}

// Hàm hỗ trợ: Đảo ngược chuỗi
static std::string reverse_str(const std::string& str) {
  return std::string(str.rbegin(), str.rend());
}

// Hàm hỗ trợ: Điều chỉnh từ theo định dạng GADDAG
static void adapt_word(int& row, int& col, bool horizontal, std::string& mot) {
  if (mot.empty()) {
    return;
  }
  std::string word;
  int r = row;
  int c = col;
  size_t i = 0;
  word += mot[i++];
  while (i < mot.size() && mot[i] != '+' &&
         moves_available(r, c, horizontal, false)) {
    word += mot[i++];
    deplacement(horizontal, false, r, c);
  }
  if (i >= mot.size() || mot[i] != '+') {
    return;
  }
  word = reverse_str(word);
  i++;
  while (i < mot.size()) {
    word += mot[i++];
  }
  row = r;
  col = c;
  mot = word;
}

// Hàm hỗ trợ: Kiểm tra từ chéo hợp lệ khi đặt một chữ cái
// Hàm hỗ trợ: Tìm chữ cái từ move tại vị trí (r, c)
static char FindMoveLetter(const Board::Move& move, int r, int c, bool& found) {
  for (const auto& placement : move) {
    if (placement.row == r && placement.col == c) {
      found = true;
      return placement.tile.letter();
    }
  }
  found = false;
  return '\0';
}

// Hàm hỗ trợ: Xây dựng từ chéo theo một hướng
static void BuildCrossWordSegment(Board& board, const Board::Move& move, int& r,
                                  int& c, bool cross_horizontal, bool forward,
                                  std::string& word) {
  while (moves_available(r, c, cross_horizontal, forward)) {
    deplacement(cross_horizontal, forward, r, c);
    if (board.IsOccupied(r, c)) {
      if (forward) {
        word.append(1, board.GetTile(r, c).letter());
      } else {
        word.insert(word.begin(), board.GetTile(r, c).letter());
      }
    } else {
      bool found = false;
      char letter = FindMoveLetter(move, r, c, found);
      if (found) {
        if (forward) {
          word.append(1, letter);
        } else {
          word.insert(word.begin(), letter);
        }
      } else if (!forward) {
        break;  // Thoát nếu không tìm thấy ô trong move khi đi ngược
      }
    }
  }
}

// Hàm kiểm tra từ chéo hợp lệ khi đặt một chữ cái
static bool VerifyCrossWords(char ch, int row, int col, Board& board,
                             Lexicon& lexicon, const Board::Move& move,
                             bool cross_horizontal) {
  std::string word(1, ch);
  int r = row;
  int c = col;

  // Xây dựng phần từ chéo theo hướng ngược
  BuildCrossWordSegment(board, move, r, c, cross_horizontal, false, word);

  // Thêm dấu '+' nếu từ có độ dài > 1
  if (word.size() > 1) {
    word += "+";
  }

  // Đặt lại vị trí và xây dựng phần từ chéo theo hướng xuôi
  r = row, c = col;
  BuildCrossWordSegment(board, move, r, c, cross_horizontal, true, word);

  // Kiểm tra từ hợp lệ
  return word.empty() || word == "+" || word.size() == 2 ||
         lexicon.Contains(word);
}

// Tìm nước đi tốt nhất từ deck_, board và lexicon
Player::Move Bot::FindBestMove(Board& board, Lexicon& lexicon, Bag& bag) {
  if (current_deck_size() == 0) {
    return PassMove();
  }
  std::vector<Player::Move> moves = GenerateAllMoves(board, lexicon);
  if (moves.empty()) {
    Player::Move swap_move = SwapTiles(bag);
    if (!swap_move.empty()) {
      // Gọi DrawNewTiles để bổ sung gạch sau khi đổi
      DrawNewTiles(bag);
      return swap_move;
    }
    return PassMove();
  }
  auto best = std::max_element(
      moves.begin(), moves.end(),
      [&board, &lexicon, this](const Player::Move& a, const Player::Move& b) {
        Player::MoveSubmissionResponse res_a = SubmitMove(a, board, lexicon);
        Player::MoveSubmissionResponse res_b = SubmitMove(b, board, lexicon);
        return res_a.move_points < res_b.move_points ||
               (res_a.move_points == res_b.move_points && a.size() > b.size());
      });
  if (best != moves.end()) {
    return *best;
  }
  return PassMove();
}

// Thêm một nước đi vào danh sách moves
void Bot::addMove(Player::Move& move, Board& board) {
  Player::MoveSubmissionResponse res_move = SubmitMove(move, board, *lexicon_);
  if (res_move.status != Board::ResponseStatus::kSuccess) {
    return;
  }

  // So sánh điểm số và số gạch sử dụng
  Player::MoveSubmissionResponse res_best =
      moves_.empty()
          ? Player::MoveSubmissionResponse(Board::MoveValidationResponse{})
          : SubmitMove(moves_.front(), board, *lexicon_);
  if (moves_.empty() || res_move.move_points > res_best.move_points ||
      (res_move.move_points == res_best.move_points &&
       move.size() < moves_.front().size())) {
    // Lưu nước đi tốt nhất
    moves_ = {move};
    // SubmitMove đã gọi AddScore nội bộ, không cần gọi lại
    // Để debug, có thể in điểm số hiện tại
    // std::cout << "Bot " << name() << " score: " << score() << std::endl;
  }
}

// Lấy danh sách các nước đi
std::vector<Player::Move> Bot::getMoves() const { return moves_; }

// Sinh tất cả nước đi hợp lệ từ deck_
std::vector<Player::Move> Bot::GenerateAllMoves(Board& board,
                                                Lexicon& lexicon) {
  std::vector<Player::Move> moves;
  std::map<char, int> freq;
  std::vector<int> indices;
  const auto& handtiles = deck();
  int index = 0;
  for (const auto& tile : handtiles) {
    if (!tile.IsEmpty()) {
      freq[tile.letter()]++;
      indices.push_back(index);
    }
    ++index;
  }
  if (board.is_first_move()) {
    MoveGenState state(std::move(freq), "", true, false,
                       std::vector<Tile>(handtiles.begin(), handtiles.end()), indices);
    GenerateWords(lexicon.getRoot(), state, board, lexicon, moves,
                  Board::kStartPosRow, Board::kStartPosColumn);
    state.orientation = false;
    GenerateWords(lexicon.getRoot(), state, board, lexicon, moves,
                  Board::kStartPosRow, Board::kStartPosColumn);
  } else {
    GenerateMoveFromAnchor(true, board, lexicon, moves);
    GenerateMoveFromAnchor(false, board, lexicon, moves);
  }
  return moves;
}

// PASS Testcase
bool Bot::IsAnchor(int row, int col, Board& board) {
  // Kiểm tra nếu ô hiện tại nằm ngoài phạm vi hoặc đã bị chiếm
  if (row < 0 || row >= Board::kHeight || col < 0 || col >= Board::kWidth || board.IsOccupied(row, col)) {
    return false;
  }

  // Kiểm tra các ô lân cận theo hàng (trên và dưới)
  for (int dr = -1; dr <= 1; dr += 2) {
    int r = row + dr;
    // Chỉ kiểm tra nếu ô lân cận nằm trong phạm vi
    if (r >= 0 && r < Board::kHeight && board.IsOccupied(r, col)) {
      return true;
    }
  }

  // Kiểm tra các ô lân cận theo cột (trái và phải)
  for (int dc = -1; dc <= 1; dc += 2) {
    int c = col + dc;
    // Chỉ kiểm tra nếu ô lân cận nằm trong phạm vi
    if (c >= 0 && c < Board::kWidth && board.IsOccupied(row, c)) {
      return true;
    }
  }

  return false;
}

// Sinh nước đi từ các điểm neo
void Bot::GenerateMoveFromAnchor(bool horizontal, Board& board,
                                 Lexicon& lexicon,
                                 std::vector<Player::Move>& moves) {
  std::map<char, int> freq;
  std::vector<int> indices;
  const auto& handtiles = deck();
  int index = 0;
  for (const auto& tile : handtiles) {
    if (!tile.IsEmpty()) {
      freq[tile.letter()]++;
      indices.push_back(index);
    }
    ++index;
  }

  for (int row = 0; row < Board::kHeight; row++) {
    for (int col = 0; col < Board::kWidth; col++) {
      if (IsAnchor(row, col, board) ||
          (board.is_first_move() && row == Board::kStartPosRow &&
           col == Board::kStartPosColumn)) {
        MoveGenState state(std::move(freq), "", horizontal, false,
                           std::vector<Tile>(handtiles.begin(), handtiles.end()),
                           indices);
        GenerateWords(lexicon.getRoot(), state, board, lexicon, moves, row,
                      col);
      }
    }
  }
}

// Kiểm tra tính hợp lệ của một nước đi
bool Bot::IsValidMove(const Player::Move& move, Board& board) {
  Board::Move board_move = ConvertToBoardMove(*this, move);
  Board::MoveValidationResponse response =
      board.ValidateMove(board_move, *lexicon_);
  return response.status == Board::ResponseStatus::kSuccess;
}

// Sinh từ một nút trong từ điển
void Bot::GenerateWords(Node* node, MoveGenState& state, Board& board,
                        Lexicon& lexicon, std::vector<Player::Move>& moves,
                        int row, int col) {
  if ((node == nullptr) || state.mot.size() > 15) {
    return;
  }
  if (!board.IsOccupied(row, col)) {
    ProcessEmptyCell(node, state, board, lexicon, moves, row, col);
  } else {
    ProcessOccupiedCell(node, state, board, lexicon, moves, row, col);
  }
}

// Xử lý ô đã có chữ cái
void Bot::ProcessOccupiedCell(Node* node, MoveGenState& state, Board& board,
                              Lexicon& lexicon,
                              std::vector<Player::Move>& moves, int row,
                              int col) {
  if (node == nullptr) {
    return;
  }
  char c = board.GetTile(row, col).letter();
  if (node->suffixes.count(c) == 0) {
    return;
  }
  const auto& handtiles = deck();
  state.mot += c;
  Node* next_node = node->suffixes[c].get();

  if (next_node->is_word && state.rack.size() < Player::kMaxDeckSize) {
    Player::Move move;
    move.push_back({-1, row, col});  // -1 vì ô đã có trên bảng

    // Xây dựng nước đi ngược từ vị trí hiện tại
    int current_row = row;
    int current_col = col;
    for (size_t i = 1; i < state.mot.size(); ++i) {
        // Lưu vị trí hiện tại trước khi di chuyển
        int prev_row = current_row;
        int prev_col = current_col;

        // Di chuyển ngược
        deplacement(state.orientation, false, current_row, current_col);

        // Kiểm tra giới hạn và tính khả dụng trước khi thêm
        if (!moves_available(current_row, current_col, state.orientation, false) ||
            current_row < 0 || current_row >= Board::kHeight ||
            current_col < 0 || current_col >= Board::kWidth) {
            break;  // Dừng nếu vượt ra ngoài hoặc không thể di chuyển
        }

        // Chỉ thêm vị trí nếu ô đó đã có chữ cái
        if (board.IsOccupied(current_row, current_col)) {
            move.insert(move.begin(), {-1, current_row, current_col});
        } else {
            // Quay lại vị trí trước đó nếu gặp ô trống
            current_row = prev_row;
            current_col = prev_col;
            break;
        }
    }
    Board::Move board_move;
    for (const auto& placement : move) {
      if (placement.tile_index != -1) {
        board_move.push_back(
            {handtiles.at(placement.tile_index), placement.row, placement.col});
      }
    }
    Board::MoveValidationResponse response =
        board.ValidateMove(board_move, lexicon);
    if (response.status == Board::ResponseStatus::kSuccess) {
      moves.push_back(move);
    }
  }

  int next_row = row + (state.orientation ? 0 : 1);
  int next_col = col + (state.orientation ? 1 : 0);
  if (moves_available(next_row, next_col, state.orientation, true)) {
    GenerateWords(next_node, state, board, lexicon, moves, next_row, next_col);
  }

  if (node->suffixes.count('+') > 0) {
    MoveGenState new_state = state;
    new_state.mot += "+";
    new_state.plus = true;
    int start_row = row;
    int start_col = col;
    adapt_word(start_row, start_col, state.orientation, new_state.mot);
    GenerateWords(node->suffixes['+'].get(), new_state, board, lexicon, moves,
                  start_row, start_col);
  }
}

// Xử lý ô trống
// Can kiem tra
void Bot::ProcessEmptyCell(Node* node, MoveGenState& state, Board& board,
                           Lexicon& lexicon, std::vector<Player::Move>& moves,
                           int& row, int& col) {
  if (node == nullptr) {
    return;
  }
  Board::Move temp_move;
  // Khong co word tren bang
  std::vector<Tile> tiles_data = {};
  board.GetWordFromPos(row, col, state.orientation, temp_move);
  NormalTileCase(node, state, board, lexicon, moves, row, col);
  BlankTileCase(node, state, board, lexicon, moves, row, col);
}

// Hàm hỗ trợ: Tạo và xác thực nước đi, thêm vào moves nếu hợp lệ
void Bot::AddValidMove(Node* node, MoveGenState& state, Board& board,
                       Lexicon& lexicon, std::vector<Player::Move>& moves,
                       int& row, int& col, int& tile_index) {
  if (!node->is_word || state.rack.size() >= Player::kMaxDeckSize) {
    return;
  }

  Player::Move move;
  move.push_back({tile_index, row, col});
  for (size_t i = 1; i < state.mot.size(); ++i) {
    int r = move[0].row;
    int c = move[0].col;
    deplacement(state.orientation, false, r, c);
    move.insert(move.begin(), {-1, r, c});
  }
  Board::Move board_move;
  const auto& handtiles_ = deck();
  for (const auto& placement : move) {
    if (placement.tile_index != -1) {
      board_move.push_back(
          {handtiles_.at(placement.tile_index), placement.row, placement.col});
    }
  }
  Board::MoveValidationResponse response =
      board.ValidateMove(board_move, lexicon);
  if (response.status == Board::ResponseStatus::kSuccess) {
    moves.push_back(move);
  }
}

void Bot::NormalTileCase(Node* node, MoveGenState& state, Board& board,
                         Lexicon& lexicon, std::vector<Player::Move>& moves,
                         int& row, int& col) {
  if (node == nullptr) {
    return;
  }
  for (const auto& [c, count] : state.freq) {
    if (count <= 0 || c == '?' || node->suffixes.count(c) == 0) {
      continue;
    }

    Board::Move temp_board_move;
    temp_board_move.push_back({Tile(c, 0), row, col});
    if (!VerifyCrossWords(c, row, col, board, lexicon, temp_board_move,
                          !state.orientation)) {
      continue;
    }

    MoveGenState new_state = state;
    new_state.mot += c;
    new_state.freq[c]--;
    int tile_index = -1;
    for (int i = 0; i < static_cast<int>(new_state.tile_indices.size()); ++i) {
      if (new_state.rack[i].letter() == c && !new_state.rack[i].IsBlankTile()) {
        tile_index = new_state.tile_indices[i];
        new_state.rack.erase(new_state.rack.begin() + i);
        new_state.tile_indices.erase(new_state.tile_indices.begin() + i);
        break;
      }
    }

    AddValidMove(node->suffixes[c].get(), new_state, board, lexicon, moves, row,
                 col, tile_index);

    int next_row = row + (state.orientation ? 0 : 1);
    int next_col = col + (state.orientation ? 1 : 0);
    if (moves_available(next_row, next_col, state.orientation, true)) {
      GenerateWords(node->suffixes[c].get(), new_state, board, lexicon, moves,
                    next_row, next_col);
    }
  }

  if (!state.plus && node->suffixes.count('+') > 0) {
    MoveGenState new_state = state;
    new_state.mot += "+";
    new_state.plus = true;
    int start_row = row;
    int start_col = col;
    adapt_word(start_row, start_col, state.orientation, new_state.mot);
    GenerateWords(node->suffixes['+'].get(), new_state, board, lexicon, moves,
                  start_row, start_col);
  }
}

void Bot::BlankTileCase(Node* node, MoveGenState& state, Board& board,
                        Lexicon& lexicon, std::vector<Player::Move>& moves,
                        int& row, int& col) {
  if (node == nullptr) {
    return;
  }
  for (const auto& [c, count] : state.freq) {
    if (count <= 0 || c == '?' || node->suffixes.count(c) == 0) {
      continue;
    }

    Board::Move temp_board_move;
    temp_board_move.push_back({Tile(c, 0), row, col});
    if (!VerifyCrossWords(c, row, col, board, lexicon, temp_board_move,
                          !state.orientation)) {
      continue;
    }

    MoveGenState new_state = state;
    new_state.mot += c;
    new_state.freq[c]--;
    int tile_index = -1;
    for (int i = 0; i < static_cast<int>(new_state.tile_indices.size()); ++i) {
      if (new_state.rack[i].IsBlankTile()) {
        tile_index = new_state.tile_indices[i];
        new_state.rack.erase(new_state.rack.begin() + i);
        new_state.tile_indices.erase(new_state.tile_indices.begin() + i);
        break;
      }
    }

    AddValidMove(node->suffixes[c].get(), new_state, board, lexicon, moves, row,
                 col, tile_index);

    int next_row = row + (state.orientation ? 0 : 1);
    int next_col = col + (state.orientation ? 1 : 0);
    if (moves_available(next_row, next_col, state.orientation, true)) {
      GenerateWords(node->suffixes[c].get(), new_state, board, lexicon, moves,
                    next_row, next_col);
    }
  }

  if (!state.plus && node->suffixes.count('+') > 0) {
    MoveGenState new_state = state;
    new_state.mot += "+";
    new_state.plus = true;
    int start_row = row;
    int start_col = col;
    adapt_word(start_row, start_col, state.orientation, new_state.mot);
    GenerateWords(node->suffixes['+'].get(), new_state, board, lexicon, moves,
                  start_row, start_col);
  }
}

// Đổi một số Tile trong deck_
Player::Move Bot::SwapTiles(Bag& bag) {
  Player::Move swap_move;
  if (current_deck_size() == 0) {
    return swap_move;
  }

  std::vector<int> indices;
  const auto& handtiles = deck();
  for (int i = 0; i < static_cast<int>(handtiles.size()); ++i) {
    if (!handtiles.at(i).IsEmpty() && handtiles.at(i).points() <= 1 &&
        indices.size() < 7) {
      indices.push_back(i);
    }
  }

  if (PerformSwap(bag, indices)) {
    for (const auto& index : indices) {
      swap_move.push_back({index, -1, -1});
    }
    // Bổ sung gạch mới sau khi đổi
    DrawNewTiles(bag);
  }

  return swap_move;
}

// Trả về nước đi bỏ lượt
Player::Move Bot::PassMove() { return Player::Move{}; }

//AutoPlaceTile
void Bot::AutoPlaceTile(Board& board, Lexicon& lexicon, Bag& bag) {
  // Tìm nước đi tốt nhất
  Player::Move best_move = FindBestMove(board, lexicon, bag);

  // Kiểm tra xem nước đi có hợp lệ không
  if (best_move.empty()) {
    std::cout << "Bot " << name() << " passes the turn (no valid move or swap).\n";
    return;
  }

  // Kiểm tra xem có phải là nước đi đổi ô chữ không
  bool is_swap_move = false;
  for (const auto& placement : best_move) {
    if (placement.row == -1 && placement.col == -1) {
      is_swap_move = true;
      break;
    }
  }

  if (is_swap_move) {
    std::vector<int> indices;
    // for (const auto& placement : best_move) {
    //   if (placement.row == -1 && placement.col == -1) {
    //     indices.push_back(placement.tile_index);
    //   }
    // }
    if (PerformSwap(bag, indices)) {
      std::cout << "Bot " << name() << " swapped " << indices.size() << " tiles.\n";
      DrawNewTiles(bag); // Rút thêm ô chữ sau khi đổi
      //PrintDeck(); // In bộ chữ cái mới để kiểm tra
    } else {
      std::cout << "Bot " << name() << " failed to swap tiles (not enough tiles in bag).\n";
    }
    return;
  }

  // Xử lý nước đi đặt ô chữ
  // Chuyển Player::Move thành Board::Move để xác thực
  const auto& handtiles = deck();
  Board::Move board_move;
  for (const auto& placement : best_move) {
    if (placement.tile_index != -1 && static_cast<size_t>(placement.tile_index) < handtiles.size()) {
      board_move.push_back({handtiles.at(placement.tile_index), placement.row, placement.col});
    }
  }

  // Xác thực nước đi
  Board::MoveValidationResponse response = board.ValidateMove(board_move, lexicon);
  if (response.status != Board::ResponseStatus::kSuccess) {
    std::cout << "Bot " << name() << " found an invalid move (validation failed).\n";
    return;
  }

  // Đặt ô chữ lên bảng
  for (const auto& placement : board_move) {
    if (board.PlaceTile(placement.tile, placement.row, placement.col)) {
      std::cout << "Bot " << name() << " placed tile " << placement.tile.letter()
                << " at (" << placement.row << ", " << placement.col << ").\n";
    } else {
      std::cout << "Bot " << name() << " failed to place tile at ("
                << placement.row << ", " << placement.col << ").\n";
      return;
    }
  }

  // Cập nhật trạng thái bot
  for (const auto& placement : best_move) {
    if (placement.tile_index != -1) {
      UseTile(placement.tile_index); // Xóa ô chữ đã sử dụng
    }
  }
  AddScore(response.move_points); // Cập nhật điểm số
  DrawNewTiles(bag); // Rút thêm ô chữ từ túi
  std::cout << "Bot " << name() << " scored " << response.move_points
            << " points. New score: " << score() << ".\n";
  PrintDeck(); // In bộ chữ cái mới
}

};  // namespace core