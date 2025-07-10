#include "dictionary.hpp"

#include <fstream>
#include <string>

namespace core {
Dictionary::Dictionary() {
  std::ifstream dict_stream("assets/dictionaries/csw_06.dict");
  dictionary.Read(&dict_stream);
}

Dictionary::Dictionary(const std::string& dict_file) {
  std::ifstream dict_stream(dict_file);
  dictionary.Read(&dict_stream);
}

bool Dictionary::Contain(const std::string& word) const {
  return dictionary.Contains(word.c_str());
}
}  // namespace core