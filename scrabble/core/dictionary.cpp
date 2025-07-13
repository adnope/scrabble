#include "dictionary.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

namespace core {
Dictionary::Dictionary() { ChangeDictionary(CSW); }

Dictionary::Dictionary(DictionaryType type) { ChangeDictionary(type); }

Dictionary::Dictionary(const std::string& dictionary_filepath) {
  LoadCustomDictionary(dictionary_filepath);
}

void Dictionary::LoadCustomDictionary(const std::string& dictionary_filepath) {
  dictionary.clear();
  std::ifstream dict_stream(dictionary_filepath);
  std::string line;
  while (std::getline(dict_stream, line)) {
    std::transform(line.begin(), line.end(), line.begin(), ::tolower);
    dictionary.emplace(line);
  }
}

void Dictionary::ChangeDictionary(DictionaryType type) {
  switch (type) {
    case CSW:
      LoadCustomDictionary("assets/dictionaries/csw6.dict");
      break;
    case TWL:
      LoadCustomDictionary("assets/dictionaries/twl6.dict");
      break;
  }
}

bool Dictionary::Contains(const std::string& word, const bool debug) const {
  if (!debug) {
    return dictionary.find(word) != dictionary.end();
  }
  if (dictionary.find(word) != dictionary.end()) {
    std::cout << "Found \"" << word << "\" in the dictionary.\n";
    return true;
  }
  std::cout << '\"' << word << "\" not found in the dictionary.\n";
  return false;
}
}  // namespace core