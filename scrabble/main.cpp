#include <chrono>
#include <iostream>

#include "SDL2/SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "core/bag.hpp"
#include "core/dictionary.hpp"

bool test_sdl2_libraries() {
  // Initialize SDL2
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << '\n';
    return false;
  }

  // Initialize SDL2_image
  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
    std::cerr << "IMG_Init Error: " << IMG_GetError() << '\n';
    SDL_Quit();
    return false;
  }

  // Initialize SDL2_ttf
  if (TTF_Init() == -1) {
    std::cerr << "TTF_Init Error: " << TTF_GetError() << '\n';
    IMG_Quit();
    SDL_Quit();
    return false;
  }

  // Create an SDL window
  constexpr int WIDTH = 800;
  constexpr int HEIGHT = 600;
  SDL_Window *window =
      SDL_CreateWindow("SDL2 Test", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << '\n';
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return false;
  }

  // Create a renderer
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << '\n';
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return false;
  }

  // Load an image using SDL2_image
  SDL_Surface *image = IMG_Load(
      "assets/images/scrabble-ui.png");  // Ensure you have this image in the working directory
  if (image == nullptr) {
    std::cerr << "IMG_Load Error: " << IMG_GetError() << '\n';
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return false;
  }

  // Create a texture from the image
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
  SDL_FreeSurface(image);  // Free the surface after creating the texture
  if (texture == nullptr) {
    std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError()
              << '\n';
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return false;
  }

  // Load font for text rendering using SDL2_ttf
  constexpr int FONT_SIZE = 24;
  TTF_Font *font =
      TTF_OpenFont("assets/fonts/Jersey15-Regular.ttf", FONT_SIZE);  // Ensure you have a font file
  if (font == nullptr) {
    std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << '\n';
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return false;
  }

  // Create a surface for text
  constexpr SDL_Color textColor = {255, 255, 255, 0};  // White text
  SDL_Surface *textSurface =
      TTF_RenderText_Solid(font, "Hello, SDL2!", textColor);
  if (textSurface == nullptr) {
    std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << '\n';
    TTF_CloseFont(font);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return false;
  }

  // Create a texture for the text
  SDL_Texture *textTexture =
      SDL_CreateTextureFromSurface(renderer, textSurface);
  SDL_FreeSurface(textSurface);  // Free the surface after creating the texture
  if (textTexture == nullptr) {
    std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError()
              << '\n';
    TTF_CloseFont(font);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return false;
  }

  // Main loop to display the image and text
  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    // Clear the screen
    SDL_RenderClear(renderer);

    // Render the image
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    // Render the text
    constexpr SDL_Rect textRect = {50, 50, 200, 50};  // Position and size of the text
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // Present the renderer
    SDL_RenderPresent(renderer);
  }

  // Cleanup and close
  SDL_DestroyTexture(textTexture);
  SDL_DestroyTexture(texture);
  TTF_CloseFont(font);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();

  return true;
}

int main() {
  auto start = std::chrono::high_resolution_clock::now();
  core::Dictionary dictionary;
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "Word count: " << dictionary.Size() << '\n';
  std::chrono::duration<double, std::milli> time_taken = end - start;
  std::cout << "Dictionary init time: " << time_taken.count() << " ms\n";

  dictionary.Contains("juxtaposition", true);

  // test_sdl2_libraries();

  core::Bag bag;
  bag.PrintBagInfo();

  auto drawn_tiles = bag.DrawTiles(7);
  for (const auto& tile : drawn_tiles) {
    tile.PrintTileInfo();
  }
  std::cout << '\n';
  bag.PrintBagInfo();
  return 0;
}