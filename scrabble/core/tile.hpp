#pragma once

namespace core {
class Tile {
 public:
  Tile() : letter{}, use{}, points{} {}

  Tile(char letter, int points) : letter(letter), use(letter), points(points) {}

  char GetLetter() const { return letter; }

  int GetPoints() const { return points; }

  bool IsBlank() const { return letter == '?'; }

  char GetUse() const { return use; }

  void UseAs(char usage) { use = usage; }

  void PrintTileInfo() const;

 private:
  char letter;
  char use;  // for blank tiles only
  int points;
};
}  // namespace core