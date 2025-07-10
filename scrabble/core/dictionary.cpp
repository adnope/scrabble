#include "dictionary.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

namespace core {
Dictionary::Dictionary() {
  LoadDictionaryTextFile("assets/dictionaries/csw_6.dict");
}

Dictionary::Dictionary(const std::string& dictionary_text_file) {
  LoadDictionaryTextFile(dictionary_text_file);
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

void Dictionary::LoadDictionaryTextFile(
    const std::string& dictionary_text_file) {
  std::ifstream dict_stream(dictionary_text_file);
  std::string line;
  while (std::getline(dict_stream, line)) {
    std::transform(line.begin(), line.end(), line.begin(), ::tolower);
    dictionary.emplace(line);
  }
}
}  // namespace core