#include "game_cli.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <sstream>

#include "cli/utils.hpp"
#include "core/player.hpp"
#include "game/game.hpp"

namespace cli {
int CLIGame::RequestNumPlayers() {
  int num_players = 0;

  while (true) {
    std::cout << "Enter number of players: ";
    std::cin >> num_players;
    if (std::cin.fail() || (num_players < 2 || num_players > 4)) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout
          << "Invalid number of players. Choose between 2 and 4 players\n";
    } else {
      break;
    }
  }

  return num_players;
}

std::vector<std::string> CLIGame::RequestPlayerNames(const int num_players) {
  std::vector<std::string> player_names;
  std::string player_name;
  std::cin.ignore();
  while (static_cast<int>(player_names.size()) != num_players) {
    while (true) {
      std::cout << "Enter player " << static_cast<int>(player_names.size()) + 1
                << " name: ";
      std::getline(std::cin, player_name);

      player_name.erase(player_name.begin(),
                        std::find_if(player_name.begin(), player_name.end(),
                                     [](char c) { return !std::isspace(c); }));
      player_name.erase(std::find_if(player_name.rbegin(), player_name.rend(),
                                     [](char c) { return !std::isspace(c); })
                            .base(),
                        player_name.end());

      if (player_name.empty() || !utils::ValidateName(player_name)) {
        std::cout << "Invalid name: \"" << player_name
                  << "\". Player names must be 3 to 14 characters long, and "
                     "only contain digits and letters.\n";
      } else {
        break;
      }
    }
    player_names.push_back(player_name);
  }
  std::cout << '\n';
  return player_names;
}

void CLIGame::InitDictionary() {
  int choice = 0;

  while (true) {
    std::cout << "Choose a dictionary for the game:\n";
    std::cout << "[1] CSW\n";
    std::cout << "[2] TWL\n";
    std::cout << "Enter your choice (1 or 2): ";

    std::cin >> choice;

    if (std::cin.fail() || (choice != 1 && choice != 2)) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "Invalid choice. Please enter 1 or 2.\n";
    } else {
      break;
    }
  }

  if (choice == 1) {
    std::cout << "Dictionary: CSW\n\n";
    game = game::Game(core::Dictionary::CSW);
  }
  if (choice == 2) {
    std::cout << "Dictionary: TWL\n\n";
    game = game::Game(core::Dictionary::TWL);
  }
}

void CLIGame::InitPlayers() {
  const int num_players = RequestNumPlayers();
  const auto player_names = RequestPlayerNames(num_players);
  for (const auto& player_name : player_names) {
    game.AddPlayer(player_name);
  }
}

void CLIGame::InitFirstPlayer() {
  constexpr int INT_MAX = 9999;
  int first_player_index = 0;
  int distance_from_a = INT_MAX;
  auto game_bag = game.bag();
  auto game_players = game.players();
  std::cout << "Determining first player...\n";
  for (int i = 0; i < game.NumPlayers(); ++i) {
    const char letter = game_bag.DrawTile().letter();
    std::cout << game_players[i].name() << ": " << letter << '\n';

    if (letter == '?') {
      std::cout << game_players[i].name() << " has a blank tile.\n";
      first_player_index = i;
      break;
    }

    if (letter - 'A' < distance_from_a) {
      first_player_index = i;
      distance_from_a = letter - 'A';
    }
  }
  std::cout << game_players[first_player_index].name() << " moves first!\n\n";
  game.SetFirstPlayer(first_player_index);
}

CLIGame::PlayerAction CLIGame::RequestPlayerAction() {
  std::cout << "Current turn: " << game.GetCurrentPlayer().name() << '\n'
            << "Your tiles: ";
  for (const auto& tile : game.GetCurrentPlayer().deck()) {
    std::cout << tile.letter() << tile.points() << " ";
  }
  std::cout << '\n';
  auto action = CLIGame::PlayerAction::kPlacing;

  int choice = 0;
  while (true) {
    std::cout << "Choose an action:\n";
    std::cout << "[1] Place\n";
    std::cout << "[2] Swap\n";
    std::cout << "[3] Pass\n";
    std::cout << "Enter your choice (1, 2 or 3): ";

    std::cin >> choice;

    if (std::cin.fail() || (choice != 1 && choice != 2 && choice != 3)) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "Invalid choice. Please enter 1, 2 or 3.\n";
    } else {
      break;
    }
  }

  if (choice == 1) {
    action = CLIGame::PlayerAction::kPlacing;
  }
  if (choice == 2) {
    action = CLIGame::PlayerAction::kSwapping;
  }
  if (choice == 3) {
    action = CLIGame::PlayerAction::kPassing;
  }

  return action;
}

// bool ValidateSwappingRequest(const std::string& request) {
//   std::istringstream iss(request);
//   int index = 0;
//   while (iss >> index) {
//     if (std::isdigit(index) == 0) {
//       return false;
//     }
//   }
// }

std::vector<int> CLIGame::RequestSwappingIndices() {
  std::cin.ignore();
  std::vector<int> indices;

  while (true) {
    std::cout << "Choose tiles to swap, separated by spaces (e.g: 1 3 7): ";

    std::string line;
    std::getline(std::cin, line);

    std::istringstream iss(line);
    int num = 0;
    bool valid = true;
    std::vector<int> temp;

    while (iss >> num) {
      if (num < 1 || num > core::Player::kMaxDeckSize ||
          game.GetCurrentPlayer().deck().at(num - 1).IsEmpty()) {
        std::cout << "Invalid tile: " << num << '\n';
        valid = false;
        break;
      }
      temp.push_back(num);
    }

    if (iss.fail() && !iss.eof()) {
      std::cout << "Invalid input. Please enter integers only.\n";
      continue;
    }

    if (valid && !temp.empty()) {
      indices = std::move(temp);
      break;
    }
    if (temp.empty()) {
      std::cout << "No valid tile chosen. Please try again.\n";
    }
  }

  return indices;
}

void CLIGame::StartGameLoop() {
  while (true) {
    if (game.IsGameOver()) {
      break;
    }

    const auto action = RequestPlayerAction();
    switch (action) {
      case CLIGame::PlayerAction::kPlacing: {
        // game.ExecutePlaceMove(player_move);
        break;
      }
      case CLIGame::PlayerAction::kSwapping: {
        const auto indices = RequestSwappingIndices();
        game.ExecuteSwapMove(indices);
        break;
      }
      case CLIGame::PlayerAction::kPassing: {
        std::cout << '\n'
                  << game.GetCurrentPlayer().name()
                  << " passed their turn!\n\n";
        game.ExecutePassMove();
        break;
      }
    }
  }
}

void CLIGame::End() {}
}  // namespace cli
