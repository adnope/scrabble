#pragma once

namespace core {
class Tile {
 public:
  Tile() : letter_{}, use_{}, points_{} {}

  Tile(const char letter, const int points)
      : letter_(letter), use_(letter), points_(points) {}

  bool IsBlankTile() const { return letter_ == '?'; }

  bool IsEmpty() const { return letter_ == 0; }

  void UseAs(const char use) { use_ = use; }

  char get_use() const { return use_; }

  char letter() const { return letter_; }

  int points() const { return points_; }

  void PrintInfo() const;

  bool operator<(const Tile &other) const{
    return letter_ < other.letter_;
  }

 private:
  char letter_;
  char use_;  // for blank tiles only
  int points_;
};
}  // namespace core