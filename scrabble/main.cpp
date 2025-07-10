#include <chrono>
#include <iostream>

#include "core/dictionary.hpp"

int main() {
  auto start = std::chrono::high_resolution_clock::now();
  core::Dictionary dictionary;
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "Word count: " << dictionary.Size() << '\n';
  std::chrono::duration<double, std::milli> time_taken = end - start;
  std::cout << "Dictionary init time: " << time_taken.count() << " ms\n";

  dictionary.Contains("juxtaposition", true);

  // // Initialize SDL2
  // if (SDL_Init(SDL_INIT_VIDEO) != 0) {
  //   std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
  //   return 1;
  // }

  // // Create SDL window
  // SDL_Window* window = SDL_CreateWindow(
  //     "SDL2 + SDL_image + SDL_ttf Test", SDL_WINDOWPOS_CENTERED,
  //     SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
  // if (!window) {
  //   std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
  //   SDL_Quit();
  //   return 1;
  // }

  // // Create renderer
  // SDL_Renderer* renderer =
  //     SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  // if (!renderer) {
  //   std::cerr << "Renderer creation failed: " << SDL_GetError() << "\n";
  //   SDL_DestroyWindow(window);
  //   SDL_Quit();
  //   return 1;
  // }

  // // Set draw color and clear screen
  // SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
  // SDL_RenderClear(renderer);
  // SDL_RenderPresent(renderer);

  // // Wait for 2 seconds
  // SDL_Delay(2000);

  // // Cleanup
  // SDL_DestroyRenderer(renderer);
  // SDL_DestroyWindow(window);
  // SDL_Quit();

  // std::cout << "SDL2, SDL2_image, and SDL2_ttf initialized and cleaned up "
  //              "successfully!\n";
  // return 0;
}