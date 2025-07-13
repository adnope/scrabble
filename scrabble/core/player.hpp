#pragma once
#include <string>
#include <vector>
#include "bag.hpp"
#include "dictionary.hpp"
#include "tile.hpp"
//#include "board.hpp"

using namespace std;

class Player {
 public:
  Player(const string &playerName, int score, int numTiles);
  ~Player();
  string getName();
  void getTiles(vector<core::Tile *>);
  void useTiles(char c);
  void exchangeTile(char c, core::Bag &bag);
  void addScore(int score);
  void subtractScore(int score);
  bool findTile(char c, int &pos);
  void returnTile(char c, std::vector<core::Tile *> &usedTiles);
  bool executePlaceMove(core::Bag &bag, core::Dictionary &dictionary, core::Board &board,
                        char dir, int row, int col, std::string word);
  void executeExchangeMove(core::Bag &bag, std::string word);

  int getScore();
  int getHandSize();
  int getHandScore();
  std::vector<core::Tile *> _handTiles;

 private:
  int _score;
  int _numTiles;
  std::string _playerName;
};
