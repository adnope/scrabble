#pragma once

#include <unordered_map>
#include <string>
#include <vector>

struct Node {
    bool is_word;
    std::unordered_map<char, Node*> suffiexes;
    void size(unsigned int & curr);
    Node();
    ~Node();
};

class Lexicon {
public:
    Lexicon(){
        root = nullptr;
        length = 0;
    }

    ~Lexicon(){
        delete root;
    }

    void AddWord(std::string word);
    bool IsContains(std::string &word)const;
    bool ContainsPrefix(const std::string &prefix) const;
    void LoadDictionaryType(DictionaryType type);
    void BuildLexiconTree();

    unsigned int size() const;
    unsigned int length;
    Node* root;
private:
    const Node* FindNode(const std::string &word) const;
    Node* NodePath(const std::string &word) const;
};
