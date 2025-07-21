#include "important.h"
#include<SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cstdio>
#include <iostream>
#include <string>
using namespace std;
// Font size
const int FONT_SIZE = 128;
const int FONT_SIZE_SMALL = 70;
// Fonts
TTF_Font* font_Jersey15_Regular ;
TTF_Font* font_aptos;
TTF_Font* font_aptos_small;
// Colors
SDL_Color white       = {255, 255, 255, 255};
SDL_Color black       = {  0,   0,   0, 255};
SDL_Color red         = {255,   0,   0, 255};
SDL_Color green       = {  0, 255,   0, 255};
SDL_Color blue        = {  0,   0, 255, 255};
SDL_Color yellow      = {255, 255,   0, 255};
SDL_Color orange      = {255, 165,   0, 255};
SDL_Color purple      = {128,   0, 128, 255};
SDL_Color gray        = {128, 128, 128, 255};
SDL_Color light_gray  = {211, 211, 211, 255};
SDL_Color brown       = {139,  69,  19, 255};
SDL_Color cyan        = {  0, 255, 255, 255};
SDL_Color magenta     = {255,   0, 255, 255};
SDL_Color pink        = {255, 192, 203, 255};
SDL_Color gold        = {255, 215,   0, 255};
SDL_Color dark_green  = {  0, 100,   0, 255};

// Text
void init_font(){
    font_Jersey15_Regular = TTF_OpenFont("assets/fonts/Jersey15-Regular.ttf", FONT_SIZE);
    if (!font_Jersey15_Regular) {
        std::cerr << "Failed to load Jersey15-Regular font: " << TTF_GetError() << std::endl;
    }
    font_aptos = TTF_OpenFont("assets/fonts/Aptos.ttf", FONT_SIZE);
    if (!font_aptos) {
        std::cerr << "Failed to load Aptos font: " << TTF_GetError() << std::endl;
    }
    font_aptos_small = TTF_OpenFont("assets/fonts/Aptos.ttf", FONT_SIZE_SMALL);
    //debug font_aptos_small
    if (!font_aptos_small) {
        std::cerr << "Failed to load Aptos font: " << TTF_GetError() << std::endl;
    }

}
Text::Text(const std::string& s, int x, int y, SDL_Renderer* renderer, SDL_Color color)
    : s(s), x(x), y(y), texture(nullptr), color(color) {
    if (!font_aptos) {
        std::cerr << "font_Aptos is NULL!" << std::endl;
        return;
    }
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font_aptos, s.c_str(), color);
    if (surface) {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        rect.x = x;
        rect.y = y;
        rect.w = surface->w;
        rect.h = surface->h;
        SDL_FreeSurface(surface);
    } else {
        rect = {x, y, 0, 0};
    }
}

void Text::print_Text(SDL_Renderer* renderer) {
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }
}

// Image
Image::Image(std::string imagePath, int x, int y, SDL_Renderer* renderer)
    : path(imagePath), x(x), y(y), texture(nullptr) {
    loadImage(renderer);
}

Image::~Image() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

bool Image::loadImage(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        printf("Failed to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        return false;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Failed to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        SDL_FreeSurface(surface);
        return false;
    }
    rect.x = x;
    rect.y = y;
    rect.w = surface->w;
    rect.h = surface->h;
    SDL_FreeSurface(surface);
    return true;
}

void Image::print_Image(SDL_Renderer* renderer) {
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }
}

bool Image::isLoaded() const {
    return texture != nullptr;
}

// Button_Text
Button_Text::Button_Text(const std::string& s, int x, int y, SDL_Renderer* renderer, SDL_Color color, bool press, TTF_Font* style)
    : s(s), x(x), y(y), texture(nullptr), color(color), press(press), style(style) {
    if (!style) {
        std::cerr << "Button_Text: Provided font style is NULL!" <<" "<< TTF_GetError() << std::endl;
        rect = {x, y, 0, 0};
        //return;
    }
    SDL_Surface* surface = TTF_RenderUTF8_Blended(style, s.c_str(), color);
    if (!surface) {
        std::cerr << "Text surface creation failed: " << TTF_GetError() << std::endl;
        rect = {x, y, 0, 0};
        return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    rect.x = x;
    rect.y = y;
    rect.w = surface->w;
    rect.h = surface->h;
    SDL_FreeSurface(surface);
}
void Button_Text::reset() {
    press = false;
}
void Button_Text::print_Text(SDL_Renderer* renderer) {
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }
}
void Button_Text::onClick() {
    press = true;
}

bool Button_Text::checkpress() {
    return press;
}

bool Button_Text::isInside(int x, int y) const {
    return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

void Button_Text::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x;
        int my = e.button.y;
        if (isInside(mx, my)) {
            onClick();
        }
    }
}

// Button_Image
Button_Image::Button_Image(std::string imagePath, int x, int y, SDL_Renderer* renderer, bool press)
    : path(imagePath), x(x), y(y), texture(nullptr), press(press) {
    loadImage(renderer);
}

Button_Image::~Button_Image() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

bool Button_Image::loadImage(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        printf("Failed to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        return false;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Failed to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        SDL_FreeSurface(surface);
        return false;
    }
    rect.x = x;
    rect.y = y;
    rect.w = surface->w;
    rect.h = surface->h;
    SDL_FreeSurface(surface);
    return true;
}

void Button_Image::onClick() {
    press = true;
}
void Button_Image::reset() {
    press = false;
}
bool Button_Image::checkpress() {
    return press;
}

bool Button_Image::isInside(int x, int y) const {
    return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

void Button_Image::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x;
        int my = e.button.y;
        if (isInside(mx, my)) {
            onClick();
        }
    }
}

void Button_Image::print_Image(SDL_Renderer* renderer) {
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }
}