#include <iostream>
#include "core/dictionary.hpp"

int main(void) {
  core::Dictionary dict;
  std::cout << std::boolalpha << dict.Contain("league") << '\n';

  return 0;
}