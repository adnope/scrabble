#include "lexicon.hpp"
#include "dictionary.hpp"

Node::Node() : is_word(false) {}
Node::~Node() {
    for (std::unordered_map<char, Node*>::iterator it = suffiexes.begin(); it != suffiexes.end(); ++it) {
        delete it->second;
    }
    suffiexes.clear();
}

void Node::size(unsigned int &curr) {
    if (is_word) {
        curr++;
    }
    for (const auto &pair : suffiexes) {
        pair.second->size(curr);
    }
}

void Lexicon::AddWord(std::string word) {
    NodePath(word)->is_word = true;
    length++;
}

bool Lexicon::IsContains(std::string &word) const {
    const Node* node = FindNode(word);
    return node != nullptr && node->is_word;
}

bool Lexicon::ContainsPrefix(const std::string &prefix) const {
    return FindNode(prefix) != nullptr;
}

const Node* Lexicon::FindNode(const std::string &word) const {
    Node* current = root;
    for (char c : word) {
        if (current->suffiexes.find(c) == current->suffiexes.end()) {
            return nullptr;
        }
        current = current->suffiexes[c];
    }
    return current;
}

Node *Lexicon::NodePath(const std::string &word) const {
    if (root == nullptr) {
        root = new Node();
    }
    Node* current = root;
    for (char c : word) {
        if (current->suffiexes.find(c) == current->suffiexes.end()) {
            current->suffiexes[c] = new Node();
        }
        current = current->suffiexes[c];
    }
    return current;
}

void Lexicon::LoadDictionaryType(DictionaryType type) {
    // Nen chuyen dictionary thanh 1 pair: <Word,Meaning>
    // Lexicon truy cap de tim tu trong dictionary, khi nguoi choi can truy cap nghia --> Hien thi Meaning
    // Tu do khong can quan tam den nghia, chi can quan tam den tu
    Dictionary dictionary;
    dictionary.LoadDictionaryType(type);
    for (const auto &word : dictionary.GetWords()) {
        AddWord(word);
    }
}

unsigned int Lexicon::size() const {
    unsigned int curr = 0;
    if (root != nullptr) {
        root->size(curr);
    }
    return curr;
}

static void GADDAG(std::string word, std::vector<std::string>& array) {
    for(unsigned int i = 0; i < word.size(); i++){
        std::pair<std::string, std::string> p = split_string(word, i);
        array.push_back(reverse_str(p.first) + "+" + p.second);
    }
}

void Lexicon::BuildLexiconTree(Dictionary& dictionary) {
    std::vector<std::string> lexicon;
    std::vector<std::string> words = dictionary.GetWords();
    for (const auto &word : words) {
        GADDAG(word, lexicon);
        for(const auto &lex : lexicon) {
            AddWord(lex);
        }
        lexicon.clear();
    }
    spdlog::info("[Lexicon] Lexicon tree built with {} words", size());
}