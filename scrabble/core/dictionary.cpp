#include "dictionary.hpp"

#include <algorithm>
#include <fstream>
#include <string>

#include "spdlog/spdlog.h"

namespace core {
Dictionary::Dictionary() { ChangeDictionary(CSW); }

Dictionary::Dictionary(const DictionaryType type) { ChangeDictionary(type); }

Dictionary::Dictionary(const std::string& dictionary_filepath) {
  LoadCustomDictionary(dictionary_filepath);
}

void Dictionary::LoadCustomDictionary(const std::string& dictionary_filepath) {
  dictionary_.clear();
  std::ifstream dict_stream(dictionary_filepath);
  if (!dict_stream.is_open()) {
    spdlog::error("[Dictionary] Cannot open dictionary file: {}",
                  dictionary_filepath);
    return;
  }
  std::string line;
  while (std::getline(dict_stream, line)) {
    std::transform(line.begin(), line.end(), line.begin(), ::tolower);
    dictionary_.emplace(line);
  }
}

void Dictionary::ChangeDictionary(const DictionaryType type) {
  switch (type) {
    case CSW:
      LoadCustomDictionary("assets/dictionaries/csw6.dict");
      break;
    case TWL:
      LoadCustomDictionary("assets/dictionaries/twl6.dict");
      break;
  }
}

bool Dictionary::Contains(const std::string& word) const {
  return dictionary_.find(word) != dictionary_.end();
}
}  // namespace core