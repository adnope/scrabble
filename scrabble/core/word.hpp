#pragma once

#include <string>
#include <utility>
#include <vector>

#include "square.hpp"

namespace core {
class Word {
 public:
  std::string AsString() const {
    std::string str;
    for (const auto& [letter, multiplier] : content_) {
      str += letter;
    }
    return str;
  }

  void AddToContent(const char letter, const Square::Multiplier multiplier) {
    content_.emplace_back(letter, multiplier);
  }

  void ApplyMultiplier() { points_ *= multiplier_; }

  void AddPoints(const int points) { points_ += points; }

  void AddMultiplier(const int multiplier) { multiplier_ *= multiplier; }

  void SetMultiplierAt(const int index, const Square::Multiplier multiplier) {
    content_[index].second = multiplier;
  }

  auto content() const { return content_; }

  int word_length() const { return static_cast<int>(content_.size()); }

  int points() const { return points_; }

 private:
  std::vector<std::pair<char, Square::Multiplier>> content_;
  int points_ = 0;
  int multiplier_ = 1;
};
}  // namespace core