#pragma once

#include <cstdint>
#include <string>
#include <unordered_set>

namespace core {

class Dictionary {
 public:
  enum DictionaryType : uint8_t { TWL, CSW };

  Dictionary();

  explicit Dictionary(DictionaryType type);

  explicit Dictionary(const std::string& dictionary_filepath);

  void ChangeDictionary(DictionaryType type);

  void LoadCustomDictionary(const std::string& dictionary_filepath);

  bool Contains(const std::string& word) const;

  size_t Size() { return dictionary.size(); }

 private:
  std::unordered_set<std::string> dictionary;
};
}  // namespace core