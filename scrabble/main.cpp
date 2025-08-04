#include "gui/gui.hpp"

int main() {
  // cli::CLIGame cli_game;
  // cli_game.Start();

  if (gui::InitSDL2()) {
    gui::GUI gui;
    gui.Start();
  }

  return 0;
}
