#pragma once

#include <string>
#include <unordered_set>

namespace core {
class Dictionary {
 public:
  Dictionary();

  explicit Dictionary(const std::string& dictionary_text_file);

  bool Contains(const std::string& word, bool debug = false) const;

  size_t Size() { return dictionary.size(); }

 private:
  std::unordered_set<std::string> dictionary;

  void LoadDictionaryTextFile(const std::string& dictionary_text_file);
};
}  // namespace core