#pragma once

namespace core {
class Tile {
 public:
  Tile() : letter_{}, use_{}, points_{} {}

  Tile(const char letter, const int points)
      : letter_(letter), use_(letter), points_(points) {}

  bool IsBlank() const { return letter_ == '?'; }

  void UseAs(const char use) { use_ = use; }

  char get_use() const { return use_; }

  char letter() const { return letter_; }

  int points() const { return points_; }

  void PrintTileInfo() const;

 private:
  char letter_;
  char use_;  // for blank tiles only
  int points_;
};
}  // namespace core