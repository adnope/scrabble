#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <fstream>
#include <string>

#include "core/dictionary.hpp"
#include "doctest/doctest.h"

TEST_CASE("dictionary test") {
  SUBCASE("csw") {
    core::Dictionary dictionary(core::Dictionary::CSW);
    std::ifstream csw6_stream("assets/dictionaries/csw6.dict");
    std::string line;
    int word_count = 0;
    int word_contained_count = 0;
    while (std::getline(csw6_stream, line)) {
      ++word_count;
      if (dictionary.Contains(line)) {
        ++word_contained_count;
      }
    }
    CHECK(word_count == word_contained_count);
  }
  SUBCASE("twl") {
    core::Dictionary dictionary(core::Dictionary::TWL);
    std::ifstream twl6_stream("assets/dictionaries/twl6.dict");
    std::string line;
    int word_count = 0;
    int word_contained_count = 0;
    while (std::getline(twl6_stream, line)) {
      ++word_count;
      if (dictionary.Contains(line)) {
        ++word_contained_count;
      }
    }
    CHECK(word_count == word_contained_count);
  }
}

