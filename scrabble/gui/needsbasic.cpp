#include <iostream>
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include "SDL_image.h"
#include "SDL_ttf.h"
using namespace std;
extern const int WIDTH = 800;
extern const int HEIGHT = 600;
extern SDL_Window *window = SDL_CreateWindow("Scrabble", SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
extern SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
extern SDL_Event e;
bool quit = false;