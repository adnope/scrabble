#ifndef START_H
#define START_H

#include "include.h"
#include "GUI.h"

// Ensure these types are defined in the included headers:
// Image, Button_Image, SDL_Event
extern Image* choose_intro;
extern Button_Image* two;
extern Button_Image* three;
extern Button_Image* four;
extern Button_Image* home;

void init_start();
void print_start(SDL_Event &e);

#endif