#pragma once

#include <string>

#include "dawgdic/dictionary.h"

namespace core {
class Dictionary {
 public:
  Dictionary();

  explicit Dictionary(const std::string& dict_file);

  bool Contain(const std::string& word) const;

 private:
  dawgdic::Dictionary dictionary;
};
}  // namespace core