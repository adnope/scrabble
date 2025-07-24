#include <iostream>
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include "SDL_image.h"
#include "SDL_ttf.h"
using namespace std;
//extern const int WIDTH = 800;
//extern const int HEIGHT = 600;
extern SDL_Window *window = SDL_CreateWindow("Scrabble", SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
extern SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
extern bool quit = false;

// constant
const SDL_DisplayMode displayMode = []{
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    return mode;
}();

extern const int WIDTH = displayMode.w;
extern const int HEIGHT = displayMode.h;