#include "lexicon.hpp"

#include <algorithm>
#include <cctype>
//#include <chrono>
#include <memory>
#include <string>
#include "dictionary.hpp"


namespace core {
Node::Node() : is_word(false) {};

Lexicon::Lexicon() : root(std::make_unique<Node>()), length(0) {}

void Node::size(unsigned int& curr) {
  curr++;
  for (const auto& suffix : suffixes) {
    if(suffix.second != nullptr) {
      suffix.second->size(curr);
    }
  }
}

void Lexicon::AddWord(const std::string& word) {
  // NodePath(word)->is_word = true;
  // length++;
  if (word.empty()) {
        return;
    }
    Node* node = NodePath(word);
    if (node == nullptr) {
        return;
    }
    node->is_word = true;
    length++;
}

bool Lexicon::Contains(std::string word) const {
  if(word.empty()) {
    return false;
  }
  std::transform(word.begin(), word.end(), word.begin(), ::tolower);
  const Node* node = FindNode(word);
  if (node == nullptr) {
    return false;
  }
  return node->is_word;
}

bool Lexicon::ContainsPrefix(const std::string& prefix) const {
  if(prefix.empty()) {
    return false;
  }
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
void GADDAG(const std::string& word, std::vector<std::string>& array) {
  for (unsigned int i = 0; i < word.size(); i++) {
    std::pair<std::string, std::string> p = split_string(word, i);
    array.push_back(reverse_str(p.first) + "+" + p.second);
  }
}

void Lexicon::BuildLexiconTree(const core::Dictionary& dictionary) { 
  //const auto start = std::chrono::high_resolution_clock::now();
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
  // const auto end = std::chrono::high_resolution_clock::now();
  // const std::chrono::duration<double, std::milli> elapsed = end - start;

  //spdlog::info("[Lexicon] Lexicon tree built with {0} words in {1} ms.",
  // size(), elapsed.count());
}

void Lexicon::PreloadDictionary(core::Dictionary::DictionaryType type) {
  core::Dictionary dictionary;
  dictionary.ChangeDictionary(type);
  BuildLexiconTree(dictionary);
  dict_type_ = type;
}

Node* Lexicon::NodePath(const std::string& word) {
  if (word.empty() || !root) {
    return nullptr;
  }

  Node* current = root.get();
  // for (char c : word) {
  //   auto it = current->suffixes.find(c);
  //   if (it == current->suffixes.end()) {
  //     current->suffixes[c] = std::make_unique<Node>();
  //   }
  //   current = current->suffixes[c].get();
  // }
  for (char c : word) {
        auto it = current->suffixes.find(c);
        if (it == current->suffixes.end()) {
            // Thay vì gán trực tiếp, chỉ tạo mới nếu chưa tồn tại
            current->suffixes[c] = std::make_unique<Node>();
        }
        current = current->suffixes[c].get();
        if (!current->is_word) { // Kiểm tra an toàn
            return nullptr;
        }
    }
  return current;
}

const Node* Lexicon::FindNode(const std::string& word) const {
  if (word.empty() || !root) {
        return nullptr;
    }
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
}  // namespace core
