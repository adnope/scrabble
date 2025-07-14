#pragma once

namespace core {
class Tile {
 public:
  Tile() : letter_{}, use_{}, points_{} {}

  Tile(char letter, int points) : letter_(letter), use_(letter), points_(points) {}

  char GetLetter() const { return letter_; }

  int GetPoints() const { return points_; }

  bool IsBlank() const { return letter_ == '?'; }

  char GetUse() const { return use_; }

  void UseAs(char use) { use_ = use; }

  void PrintTileInfo() const;

 private:
  char letter_;
  char use_;  // for blank tiles only
  int points_;
};
}  // namespace core