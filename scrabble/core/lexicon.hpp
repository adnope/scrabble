#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "core/dictionary.hpp"

namespace core {
struct Node {
  bool is_word;
  std::unordered_map<char, std::unique_ptr<Node>> suffixes;
  void size(unsigned int& curr);

  Node();
  ~Node() = default;
  Node(Node&&) = default;
  Node& operator=(Node&&) = default;

  Node(const Node&) = delete;
  Node& operator=(const Node&) = delete;

  void serialize(std::ofstream& ofs) const;
  static std::unique_ptr<Node> deserialize(std::ifstream& ifs);
};

class Lexicon {
 public:
  Lexicon();
  ~Lexicon() = default;
  Lexicon(Lexicon&&) = default;
  Lexicon& operator=(Lexicon&&) = default;
  Lexicon(const Lexicon&) = delete;
  Lexicon& operator=(const Lexicon&) = delete;

  void AddWord(const std::string& word);
  bool Contains(std::string word) const;
  bool ContainsPrefix(const std::string& prefix) const;
  void PreLoadDictionary(core::Dictionary::DictionaryType type);
  void BuildLexiconTree(const core::Dictionary& dictionary);

  unsigned int size() const;

 private:
  std::unique_ptr<Node> root;
  const Node* FindNode(const std::string& word) const;
  Node* NodePath(const std::string& word);
  unsigned int length;
};

}  // namespace core
