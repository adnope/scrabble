#include "bot.hpp"

#include <unistd.h>

#include <algorithm>
#include <string>

#include "bag.hpp"
#include "core/board.hpp"
#include "core/tile.hpp"

namespace core {

Bot::Bot(const std::string& player_name, int score)
    : Player(player_name, score), lexicon_(std::make_unique<Lexicon>()) {
  // Cần một Lexicon hợp lệ, ví dụ:
  lexicon_->PreloadDictionary(core::Dictionary::DictionaryType::CSW);
}

// Hàm hỗ trợ: Kiểm tra xem có thể di chuyển từ vị trí (row, col) theo hướng và
// chiều
static bool moves_available(int row, int col, bool horizontal, bool plus) {
  // Kiểm tra vị trí có nằm trong biên bảng không
  if (row < 0 || row >= Board::kHeight || col < 0 || col >= Board::kWidth) {
    return false;
  }

  if (horizontal) {
    return plus ? (col < Board::kWidth - 1) : (col > 0);
  }

  return plus ? (row < Board::kHeight - 1) : (row > 0);
}

// Hàm hỗ trợ: Cập nhật vị trí (row, col) theo hướng và chiều
static void deplacement(bool horizontal, bool plus, int& row, int& col) {
  if (horizontal) {
    if (plus && col < Board::kWidth - 1) {
      col++;
    } else if (!plus && col > 0) {
      col--;
    }
  } else {
    if (plus && row < Board::kHeight - 1) {
      row++;
    } else if (!plus && row > 0) {
      row--;
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
  while (i < mot.size() && mot[i] != '+' && r >= 0 && r < Board::kHeight &&
         c >= 0 && c < Board::kWidth &&
         moves_available(r, c, horizontal, false)) {
    word += mot[i++];
    deplacement(horizontal, false, r, c);
    if (r < 0 || r >= Board::kHeight || c < 0 || c >= Board::kWidth) {
      break;  // Vượt ra ngoài phạm vi bảng
    }
  }

  if (r < 0 || r >= Board::kHeight || c < 0 || c >= Board::kWidth) {
    return;  // Vượt ra ngoài phạm vi bảng
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
  while (moves_available(r, c, cross_horizontal, forward) &&
         (r >= 0 && r < Board::kHeight && c >= 0 && c < Board::kWidth)) {
    deplacement(cross_horizontal, forward, r, c);
    if (r < 0 || r >= Board::kHeight || c < 0 || c >= Board::kWidth) {
      break;
    }
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

Player::Move Bot::FindBestMove(Board& board, Lexicon& lexicon, Bag& bag) {
  if (current_deck_size() == 0) {
    return PassMove();
  }

  // Tạo danh sách tất cả các nước đi có thể
  std::vector<Player::Move> moves = GenerateAllMoves(board, lexicon);

  moves.erase(
      std::remove_if(moves.begin(), moves.end(),
                     [](const Player::Move& move) {
                       return std::any_of(
                           move.begin(), move.end(), [](const Placement& p) {
                             return p.row < 0 || p.row >= Board::kHeight ||
                                    p.col < 0 || p.col >= Board::kWidth;
                           });
                     }),
      moves.end());

  if (moves.empty()) {
    Player::Move swap_move = SwapTiles(bag);
    if (!swap_move.empty()) {
      // Bổ sung gạch sau khi đổi
      DrawNewTiles(bag);
      return swap_move;
    }
    return PassMove();
  }

  // Tìm nước đi tốt nhất dựa trên điểm số
  Player::Move best_move;
  int best_score = -1;

  for (const auto& move : moves) {
    // Chuyển đổi sang Board::Move sử dụng move state thực tế
    Board::Move board_move;
    const auto& handtiles = deck();  // Lấy danh sách tile hiện tại
    for (const auto& placement : move) {
      // Lấy tile từ deck theo chỉ số
      if (placement.tile_index >= 0 &&
          static_cast<size_t>(placement.tile_index) < handtiles.size()) {
        Tile tile = handtiles.at(placement.tile_index);

        // Xử lý blank tile
        if (tile.IsBlankTile()) {
          Tile new_tile = tile;           // Tạo bản sao
          new_tile.UseAs(placement.use);  // Gán ký tự sử dụng
          board_move.push_back({new_tile, placement.row, placement.col});
        } else {
          board_move.push_back({tile, placement.row, placement.col});
        }
      }
    }

    // Xác thực nước đi
    auto response = board.ValidateMove(board_move, lexicon);

    // Chọn nước đi có điểm cao nhất
    if (response.status == Board::ResponseStatus::kSuccess &&
        response.move_points > best_score) {
      best_score = response.move_points;
      best_move = move;
    }
  }

  if (best_score > 0) {
    return best_move;
  }

  return PassMove();
}
// Lấy danh sách các nước đi
std::vector<Player::Move> Bot::getMoves() const { return moves_; }

// OK
//  Sinh tất cả nước đi hợp lệ từ deck_
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
                       std::vector<Tile>(handtiles.begin(), handtiles.end()),
                       indices);
    GenerateWords(lexicon.getRoot(), state, board, lexicon, moves,
                  Board::kStartPosRow, Board::kStartPosColumn);
    state.orientation = false;
    state.freq = freq;  // Khôi phục freq vì state đã bị move
    state.rack = std::vector<Tile>(handtiles.begin(), handtiles.end());
    state.tile_indices = indices;
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
  if (row < 0 || row >= Board::kHeight || col < 0 || col >= Board::kWidth ||
      board.IsOccupied(row, col)) {
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

// OK
//  Sinh nước đi từ các điểm neo
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
        MoveGenState state(
            std::move(freq), "", horizontal, false,
            std::vector<Tile>(handtiles.begin(), handtiles.end()), indices);
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

// OK
//  Sinh từ một nút trong từ điển
void Bot::GenerateWords(Node* node, MoveGenState& state, Board& board,
                        Lexicon& lexicon, std::vector<Player::Move>& moves,
                        int row, int col) {
  if ((node == nullptr) || state.mot.size() > 15) {
    return;
  }
  if (row < 0 || row >= Board::kHeight || col < 0 || col >= Board::kWidth) {
    return;
  }
  if (!board.IsOccupied(row, col)) {
    ProcessEmptyCell(node, state, board, lexicon, moves, row, col);
  } else {
    ProcessOccupiedCell(node, state, board, lexicon, moves, row, col);
  }
}

// OK
// Xử lý ô đã có chữ cái
void Bot::ProcessOccupiedCell(Node* node, MoveGenState& state, Board& board,
                              Lexicon& lexicon,
                              std::vector<Player::Move>& moves, int row,
                              int col) {
  if (node == nullptr) {
    return;
  }
  if (row < 0 || row >= Board::kHeight || col < 0 || col >= Board::kWidth) {
    return;
  }
  char c = board.GetTile(row, col).letter();
  if (node->suffixes.count(c) == 0) {
    return;
  }
  // const auto& handtiles = deck();
  state.mot += c;
  state.current_move.push_back(
      {-1, c, row, col});  // -1 indicates no tile index
  Node* next_node = node->suffixes[c].get();
  Board::Move temp_move;
  if (!VerifyCrossWords(c, row, col, board, lexicon, temp_move,
                        !state.orientation)) {
    state.mot.pop_back();
    state.current_move.pop_back();
    return;
  }

  int next_row = row + (state.orientation ? 0 : 1);
  int next_col = col + (state.orientation ? 1 : 0);
  if (moves_available(next_row, next_col, state.orientation, true)) {
    GenerateWords(next_node, state, board, lexicon, moves, next_row, next_col);
  }

  if (node->suffixes.count('+') > 0 && !state.plus) {
    MoveGenState new_state = state;
    new_state.mot += "+";
    new_state.plus = true;
    int start_row = row;
    int start_col = col;
    adapt_word(start_row, start_col, state.orientation, new_state.mot);
    GenerateWords(node->suffixes['+'].get(), new_state, board, lexicon, moves,
                  start_row, start_col);
  }
  state.mot.pop_back();
  state.current_move.pop_back();
}

// OK
// Can kiem tra
void Bot::ProcessEmptyCell(Node* node, MoveGenState& state, Board& board,
                           Lexicon& lexicon, std::vector<Player::Move>& moves,
                           int& row, int& col) {
  if (node == nullptr) {
    return;
  }
  NormalTileCase(node, state, board, lexicon, moves, row, col);
  BlankTileCase(node, state, board, lexicon, moves, row, col);
}

void Bot::NormalTileCase(Node* node, MoveGenState& state, Board& board,
                         Lexicon& lexicon, std::vector<Player::Move>& moves,
                         int& row, int& col) {
  if (node == nullptr) {
    return;
  }
  for (const auto& [c, count] : state.freq) {
    if (count <= 0 || node->suffixes.count(c) == 0) {
      continue;
    }
    int tile_index = -1;
    for (int i = 0; i < static_cast<int>(state.tile_indices.size()); ++i) {
      if (state.rack[i].letter() == c && !state.rack[i].IsBlankTile()) {
        tile_index = state.tile_indices[i];
        break;
      }
    }

    if (tile_index == -1) {
      continue;
    }

    ProcessTile(node, state, board, lexicon, moves, row, col, c, tile_index);
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

    int tile_index = -1;
    for (int i = 0; i < static_cast<int>(state.tile_indices.size()); ++i) {
      if (state.rack[i].letter() == c && !state.rack[i].IsBlankTile()) {
        tile_index = state.tile_indices[i];
        break;
      }
    }

    if (tile_index == -1) {
      continue;
    }

    ProcessTile(node, state, board, lexicon, moves, row, col, c, tile_index);
  }

  // Phần 2: Xử lý blank tile (thử các ký tự từ 'a' đến 'z')

  for (char c = 'a'; c <= 'z'; ++c) {
    if (node->suffixes.count(c) == 0) {
      continue;
    }

    int tile_index = -1;
    for (int i = 0; i < static_cast<int>(state.tile_indices.size()); ++i) {
      if (state.rack[i].IsBlankTile()) {
        tile_index = state.tile_indices[i];
        break;
      }
    }

    if (tile_index == -1) {
      continue;
    }

    ProcessTile(node, state, board, lexicon, moves, row, col, c, tile_index);
  }

  // Phần 3: Xử lý dấu '+' trong GADDAG
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

// AutoPlaceTile
void Bot::AutoPlaceTile(Board& board, Lexicon& lexicon, Bag& bag) {
  // Tìm nước đi tốt nhất
  Player::Move best_move = FindBestMove(board, lexicon, bag);

  // Kiểm tra xem nước đi có hợp lệ không
  if (best_move.empty()) {
    std::cout << "Bot " << name()
              << " passes the turn (no valid move or swap).\n";
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
      std::cout << "Bot " << name() << " swapped " << indices.size()
                << " tiles.\n";
      DrawNewTiles(bag);  // Rút thêm ô chữ sau khi đổi
      // PrintDeck(); // In bộ chữ cái mới để kiểm tra
    } else {
      std::cout << "Bot " << name()
                << " failed to swap tiles (not enough tiles in bag).\n";
    }
    return;
  }

  // Xử lý nước đi đặt ô chữ
  // Chuyển Player::Move thành Board::Move để xác thực
  const auto& handtiles = deck();
  Board::Move board_move;
  for (const auto& placement : best_move) {
    if (placement.tile_index != -1 &&
        static_cast<size_t>(placement.tile_index) < handtiles.size()) {
      board_move.push_back(
          {handtiles.at(placement.tile_index), placement.row, placement.col});
    }
  }

  // Xác thực nước đi
  Board::MoveValidationResponse response =
      board.ValidateMove(board_move, lexicon);
  if (response.status != Board::ResponseStatus::kSuccess) {
    std::cout << "Bot " << name()
              << " found an invalid move (validation failed).\n";
    return;
  }

  // Đặt ô chữ lên bảng
  for (const auto& placement : board_move) {
    if (board.PlaceTile(placement.tile, placement.row, placement.col)) {
      std::cout << "Bot " << name() << " placed tile "
                << placement.tile.letter() << " at (" << placement.row << ", "
                << placement.col << ").\n";
    } else {
      std::cout << "Bot " << name() << " failed to place tile at ("
                << placement.row << ", " << placement.col << ").\n";
      return;
    }
  }

  // Cập nhật trạng thái bot
  for (const auto& placement : best_move) {
    if (placement.tile_index != -1) {
      UseTile(placement.tile_index);  // Xóa ô chữ đã sử dụng
    }
  }
  AddScore(response.move_points);  // Cập nhật điểm số
  DrawNewTiles(bag);               // Rút thêm ô chữ từ túi
  std::cout << "Bot " << name() << " scored " << response.move_points
            << " points. New score: " << score() << ".\n";
  PrintDeck();  // In bộ chữ cái mới
}

// Hàm phụ trợ: Xử lý logic chung cho normal tile và blank tile
void Bot::ProcessTile(Node* node, MoveGenState& state, Board& board,
                      Lexicon& lexicon, std::vector<Player::Move>& moves,
                      int row, int col, char c, int tile_index) {
  if (row < 0 || row >= Board::kHeight || col < 0 || col >= Board::kWidth) {
    return;
  }
  Board::Move temp_board_move;
  temp_board_move.push_back({Tile(c, 0), row, col});
  if (!VerifyCrossWords(c, row, col, board, lexicon, temp_board_move,
                        !state.orientation)) {
    return;
  }

  MoveGenState new_state = state;
  new_state.mot += c;
  if (c == '?') {
    new_state.freq['?']--;
  } else {
    new_state.freq[c]--;
  }
  if (tile_index != -1) {
    for (int i = 0; i < static_cast<int>(new_state.tile_indices.size()); ++i) {
      if (new_state.tile_indices[i] == tile_index) {
        new_state.rack.erase(new_state.rack.begin() + i);
        new_state.tile_indices.erase(new_state.tile_indices.begin() + i);
        break;
      }
    }
  }

  new_state.current_move.push_back({tile_index, c, row, col});

  if (node->suffixes[c]->is_word && !new_state.mot.empty()) {
    Board::Move board_move;
    const auto& handtiles =
        state.rack;  // Sử dụng rack từ state để đảm bảo tính nhất quán
    for (const auto& placement : new_state.current_move) {
      if (placement.tile_index != -1) {
        board_move.push_back(
            {handtiles.at(placement.tile_index), placement.row, placement.col});
      }
    }
    Board::MoveValidationResponse response =
        board.ValidateMove(board_move, lexicon);
    if (response.status == Board::ResponseStatus::kSuccess) {
      moves.push_back(new_state.current_move);
    }
  }

  int next_row = row + (state.orientation ? 0 : 1);
  int next_col = col + (state.orientation ? 1 : 0);
  if (moves_available(next_row, next_col, state.orientation, true)) {
    GenerateWords(node->suffixes[c].get(), new_state, board, lexicon, moves,
                  next_row, next_col);  // Thử lại tại vị trí ban đầu
  }
}

};  // namespace core