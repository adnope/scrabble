#include "lexicon.hpp"

#include <sys/types.h>

#include <algorithm>
#include <cctype>
#include <core/lexicon.hpp>
#include <fstream>
#include <memory>
#include <string>

#include "dictionary.hpp"
#include "spdlog/spdlog.h"

namespace core {

Node::Node() : is_word(false) {};

Lexicon::Lexicon() : root(std::make_unique<Node>()), length(0) {}

void Node::size(unsigned int& curr) {
  curr++;
  for (const auto& suffix : suffixes) {
    suffix.second->size(curr);
  }
}

void Lexicon::AddWord(const std::string& word) {
  NodePath(word)->is_word = true;
  length++;
}

bool Lexicon::Contains(std::string word) const {
  std::transform(word.begin(), word.end(), word.begin(), ::tolower);
  const Node* node = FindNode(word);
  if (node == nullptr) {
    return false;
  }
  return node->is_word;
}

bool Lexicon::ContainsPrefix(const std::string& prefix) const {
  return FindNode(prefix) != nullptr;
}

// split_string
static std::pair<std::string, std::string> split_string(const std::string& str,
                                                        size_t index) {
  return {str.substr(0, index), str.substr(index)};
}
// reverse_str
static std::string reverse_str(const std::string& str) {
  return std::string(str.rbegin(), str.rend());
}

// Done
static void GADDAG(const std::string& word, std::vector<std::string>& array) {
  for (unsigned int i = 0; i < word.size(); i++) {
    std::pair<std::string, std::string> p = split_string(word, i);
    array.push_back(reverse_str(p.first) + "+" + p.second);
  }
}

void Lexicon::BuildLexiconTree(const core::Dictionary& dictionary) {
  std::vector<std::string> lexicon;
  std::vector<std::string> words = dictionary.GetWords();
  for (const auto& word : words) {
    // Tu goc
    AddWord(word);
    // Tu bien the GADDAG
    GADDAG(word, lexicon);
    for (const auto& lex : lexicon) {
      AddWord(lex);
    }
    lexicon.clear();
  }
  spdlog::info("[Lexicon] Lexicon tree built with {} words", size());
}

void Lexicon::PreLoadDictionary(core::Dictionary::DictionaryType type) {
  core::Dictionary dictionary;
  dictionary.ChangeDictionary(type);
  BuildLexiconTree(dictionary);
}

Node* Lexicon::NodePath(const std::string& word) {
  if (word.empty()) {
    return root.get();
  }

  Node* current = root.get();
  for (char c : word) {
    auto it = current->suffixes.find(c);
    if (it == current->suffixes.end()) {
      current->suffixes[c] = std::make_unique<Node>();
    }
    current = current->suffixes[c].get();
  }
  return current;
}

const Node* Lexicon::FindNode(const std::string& word) const {
  const Node* current = root.get();
  for (char c : word) {
    auto it = current->suffixes.find(c);
    if (it == current->suffixes.end()) {
      return nullptr;
    }
    current = it->second.get();
  }
  return current;
}

unsigned int Lexicon::size() const {
  unsigned int curr = 0;
  if (root != nullptr) {
    root->size(curr);
  }
  return curr;
}

void Node::serialize(std::ofstream& ofs) const {
  ofs << is_word << " " << suffixes.size() << " ";
  for (const auto& suffix : suffixes) {
    ofs << suffix.first << " ";
    if (suffix.second) {
      suffix.second->serialize(ofs);
    }
  }
}

std::unique_ptr<Node> Node::deserialize(std::ifstream& ifs) {
  auto node = std::make_unique<Node>();
  ifs >> node->is_word;
  size_t suffix_count = 0;
  ifs >> suffix_count;
  for (size_t i = 0; i < suffix_count; ++i) {
    char c = 0;
    ifs >> c;
    node->suffixes[c] = Node::deserialize(ifs);
  }
  return node;
}

}  // namespace core
