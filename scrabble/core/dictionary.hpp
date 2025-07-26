#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace core {

class Dictionary {
 public:
  enum DictionaryType : uint8_t { TWL, CSW };

  Dictionary();

  explicit Dictionary(DictionaryType type);

  explicit Dictionary(const std::string& dictionary_filepath);

  void ChangeDictionary(DictionaryType type);

  void LoadCustomDictionary(const std::string& dictionary_filepath);

  // bool Contains(const std::string& word) const;

  size_t size() const { return dictionary_.size(); }
  const std::vector<std::string>& GetWords() const { return dictionary_; }

 private:
  std::vector<std::string> dictionary_;
};
}  // namespace core
