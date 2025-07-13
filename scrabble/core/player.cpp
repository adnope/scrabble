#include <vector>
#include <string>
#include <iostream>
#include <set>
#include "bag.hpp"
#include "dictionary.hpp"
#include "tile.hpp"
#include "player.hpp"
// #include "square.hpp"
// #include "board.hpp"

using namespace std;

Player::Player(const string &playerName, int score, int numTiles) {
    _playerName = playerName;
    _score = score;
    _numTiles = numTiles;
}

Player::~Player() {
    // Destructor player
    for(int i = 0; i < _handTiles.size(); ++i) {
        delete _handTiles[i];
    }
}

// Hàm này sẽ trả về số điểm của người chơi 
string Player::getName() {
    return _playerName;
}

// Hàm này sẽ trả về số điểm của người chơi 
void Player::getTiles(vector<core::Tile *> tiles) {
    for (auto tile : tiles) {
        _handTiles.push_back(tile);
    }
}


// Hàm này sẽ sử dụng viên gạch trong tay người chơi
void Player::useTiles(char c) {
    int pos;
    for(int i=0; i<_handTiles.size(); ++i) {
        //need getLetter() function in Tile class
        if(c == _handTiles[i]->getLetter()){
			_handTiles.erase(_handTiles.begin() + i);
			break;
		}
    }
}

// Hàm này sẽ tìm vị trí của viên gạch trong tay người chơi
bool Player::findTile(char c, int &pos) {
    for(int i=0; i<_handTiles.size(); ++i) {
        if i == pos
            continue;
        //need getLetter() function in Tile class
        if(c == _handTiles[i]->getLetter()) {
            pos = i;
            return true;
        }
    }
    return false;
}

// Hàm này sẽ đưa viên gạch trong tay người chơi vào vector các viên gạch đã sử dụng
// Nếu từ trên ô hợp lệ -> xóa khỏi tay người chơi 
// Khi đó usedTiles sẽ chứa các viên gạch đã sử dụng sẽ sử dụng trong tính điểm và xóa khỏi tay người chơi
void Player::returnTile(char c, std::vector<core::Tile *> &usedTiles) {
    int pos;
    if (findTile(c, pos)) {
        usedTiles.push_back(_handTiles[pos]);
        _handTiles.erase(_handTiles.begin() + pos);
    }
}

//Hàm này sẽ trao đổi một viên gạch với bag
void Player::exchangeTile(char c, core::Bag &bag) {
    int pos;
    if (findTile(c, pos)) {
        // Funtion AddTiles in Bag class nó là AddTiles(const Tile& tile), AddTiles(const std::vector<Tile>& tiles)
        // nên mình sẽ dùng AddTiles(const Tile& tile) để thêm từng viên vào bag
        // hiện tại nó không nhận 1 phẩn từ có thể là do nhầm 2 funtion với nhau
        bag.AddTiles(_handTiles[pos]);
        _handTiles.erase(_handTiles.begin() + pos);
    }
}


void Player::addScore(int score) {
    _score += score;
}

void Player::subtractScore(int score) {
    _score -= score;
}

int Player::getScore() {
    return _score;
}

int Player::getHandSize() {
    return _handTiles.size();
}

int Player::getHandScore() {
    int handScore = 0;
    for (const auto &tile : _handTiles) {
        //Need getPoints() function in Tile class
        handScore += tile->getPoints();
    }
    return handScore;
}

void Player::executeExchangeMove(core::Bag &bag, std::string word) {
    // Implementation of place move logic
    for(int i = 0; i < (int)word.length(); ++i) {
        this->exchangeTile(word[i], bag);
    }
    this->getTiles(bag.DrawTiles(word.length()));
}

