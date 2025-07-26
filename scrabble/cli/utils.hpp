#pragma once

#include <algorithm>
#include <string>

namespace cli::utils {
bool ValidateName(const std::string& player_name) {
  constexpr int kMinNameLength = 3;
  constexpr int kMaxNameLength = 14;
  if (player_name.size() < kMinNameLength ||
      player_name.size() > kMaxNameLength) {
    return false;
  }

  return std::all_of(player_name.begin(), player_name.end(),
                     [](const char c) { return isalnum(static_cast<int>(c)); });
}
}  // namespace cli::utils