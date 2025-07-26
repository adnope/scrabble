#include "game_cli.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <ios>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

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

void CLIGame::PrintSwappingIndices(const std::vector<int>& indices) {
  if (indices.size() == 1) {
    std::cout << game.current_player().deck().at(indices[0]).letter();
    return;
  }
  if (indices.size() == 2) {
    std::cout << game.current_player().deck().at(indices[0]).letter() << " and "
              << game.current_player().deck().at(indices[1]).letter();
    return;
  }
  for (size_t i = 0; i < indices.size() - 2; ++i) {
    std::cout << game.current_player().deck().at(indices[i]).letter() << ", ";
  }
  std::cout << game.current_player().deck().at(*(indices.end() - 2)).letter()
            << " and "
            << game.current_player().deck().at(*(indices.end() - 1)).letter();
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
  for (int i = 0; i < game.num_players(); ++i) {
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

void CLIGame::PrintMoveHistory() {
  const auto move_history = game.move_history();
  int turn_number = static_cast<int>(move_history.size());
  for (auto it = move_history.rbegin(); it != move_history.rend();
       ++it, --turn_number) {
    const auto& move = *it;
    std::cout << turn_number << ". ";
    if (move.type == game::Game::MoveType::kPassing) {
      std::cout << "PASS: " << move.player_name << " passed their turn\n";
    }
    if (move.type == game::Game::MoveType::kSwapping) {
      std::cout << "SWAP: " << move.player_name << " swapped tiles\n";
    }
    if (move.type == game::Game::MoveType::kPlacing) {
      std::cout << "PLACE: " << move.player_name << " earned " << move.points
                << " points\nWords formed: ";
      for (const auto& word : move.words) {
        std::cout << word.AsString() << " ";
      }
      std::cout << '\n';
    }
  }
}

CLIGame::PlayerAction CLIGame::RequestPlayerAction() {
  game.PrintBoard();

  std::cout << "\nCurrent turn: " << game.current_player().name() << '\n'
            << "Your tiles: ";
  for (const auto& tile : game.current_player().deck()) {
    std::cout << tile.letter() << tile.points() << " ";
  }
  std::cout << '\n';

  std::cout << "Consecutive passes: " << game.consecutive_passes() << '\n';

  PrintMoveHistory();

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

bool CLIGame::ValidateSwappingIndicesInput(const std::string& input,
                                           std::vector<int>& indices) {
  if (input.empty() || std::all_of(input.begin(), input.end(), ::isspace)) {
    std::cout << "No tile selected, please try again.\n";
    return false;
  }

  std::istringstream iss(input);
  std::string token;
  std::vector<int> temp;
  std::array<bool, 7> seen = {};

  while (iss >> token) {
    if (!std::all_of(token.begin(), token.end(), ::isdigit)) {
      std::cout << "Invalid tile: " << token << '\n';
      return false;
    }

    int index = std::stoi(token);
    if (index < 1 || index > 7) {
      std::cout << "Invalid tile: " << index << '\n';
      return false;
    }

    if (game.current_player().deck().at(index - 1).IsEmpty()) {
      std::cout << "Slot " << index << " is empty.\n";
      return false;
    }

    if (seen.at(index - 1)) {
      std::cout << "Duplicated tile: " << index << '\n';
      return false;
    }

    seen.at(index - 1) = true;
    temp.push_back(index);
  }

  indices = std::move(temp);
  return true;
}

std::vector<int> CLIGame::RequestSwappingIndices() {
  std::cin.ignore();
  std::vector<int> indices;

  while (true) {
    std::cout << "Choose tiles to swap, separated by spaces (e.g: 1 3 7): ";

    std::string line;
    std::getline(std::cin, line);
    
    if (ValidateSwappingIndicesInput(line, indices)) {
      break;
    }
  }

  return indices;
}

void CLIGame::StartGameLoop() {
  while (true) {
    if (game.IsOver()) {
      break;
    }

    while (true) {
      bool valid_action = false;

      const auto action = RequestPlayerAction();
      switch (action) {
        case CLIGame::PlayerAction::kPlacing: {
          // game.ExecutePlaceMove(player_move);
          break;
        }
        case CLIGame::PlayerAction::kSwapping: {
          valid_action = ExecuteSwapMove();
          break;
        }
        case CLIGame::PlayerAction::kPassing: {
          valid_action = ExecutePassMove();
          break;
        }
      }

      if (valid_action) {
        break;
      }
    }
  }
}

bool CLIGame::ExecutePassMove() {
  std::cout << '\n'
            << game.current_player().name() << " passed their turn!\n\n";
  game.ExecutePassMove();
  return true;
}

bool CLIGame::ExecuteSwapMove() {
  const auto indices = RequestSwappingIndices();
  const auto response = game.ExecuteSwapMove(indices);

  if (!response.sucessful) {
    std::cout << "There's not enough tiles remaining in bag for the swap!\n";
    return false;
  }

  std::cout << "You swapped tiles: ";
  for (const auto& tile : response.old_tiles) {
    std::cout << tile << " ";
  }
  std::cout << "for ";
  for (const auto& tile : response.new_tiles) {
    std::cout << tile << " ";
  }
  std::cout << '\n';

  std::cout << "Your tiles now: " << response.deck << "\n\n";

  return true;
}

void CLIGame::End() {}
}  // namespace cli
