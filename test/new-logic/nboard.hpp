#pragma once

#include <array>

#include "core/square.hpp"

namespace nl {
class Nboard {
 public:
  static constexpr int BOARD_WIDTH = 15;
  static constexpr int BOARD_HEIGHT = 15;
  static constexpr int START_ROW = 7;
  static constexpr int START_COL = 7;

 private:
  std::array<std::array<core::Square, BOARD_WIDTH>, BOARD_HEIGHT> board_grid;
};
}  // namespace nl
