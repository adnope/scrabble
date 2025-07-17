#ifndef IMPORTANT_H
#define IMPORTANT_H

// Add your includes here
#include "include.h"
// Add your declarations and definitions here
// fonts
extern TTF_Font* font_Jersey15_Regular;
extern TTF_Font* font_aptos;

// font size
extern const int FONT_SIZE;

// colors
extern SDL_Color white;
extern SDL_Color black;
extern SDL_Color red;
extern SDL_Color green;
extern SDL_Color blue;
extern SDL_Color yellow;
extern SDL_Color orange;
extern SDL_Color purple;
extern SDL_Color gray;
extern SDL_Color light_gray;
extern SDL_Color brown;
extern SDL_Color cyan;
extern SDL_Color magenta;
extern SDL_Color pink;
extern SDL_Color gold;
extern SDL_Color dark_green;

void init_font();
// class declarations
class Text {
public:
    Text(const std::string& s, int x, int y, SDL_Renderer* renderer, SDL_Color color = white);
    void print_Text(SDL_Renderer* renderer);

protected:
    SDL_Color color;
    std::string s;
    int x, y;
    SDL_Rect rect;
    SDL_Texture* texture;
};

class Image {
public:
    Image(std::string imagePath, int x, int y, SDL_Renderer* renderer);
    ~Image();
    bool loadImage(SDL_Renderer* renderer);
    void print_Image(SDL_Renderer* renderer);
    bool isLoaded() const;
    
protected:
    std::string path;
    int x, y;
    SDL_Rect rect;
    SDL_Texture* texture;
};

class Button_Text {
public:
    Button_Text(const std::string& s, int x, int y, SDL_Renderer* renderer, SDL_Color color = white, bool press = false);
    bool checkpress();
    void handleEvent(const SDL_Event& e);
    void onClick();
    bool isInside(int x, int y) const;
    void print_Text(SDL_Renderer* renderer);
    void reset();  // Reset the button state
protected:
    std::string s;
    int x, y;
    SDL_Rect rect;
    SDL_Texture* texture;
    SDL_Color color;
    bool press;
};

class Button_Image {
public:
    Button_Image(std::string imagePath, int x, int y, SDL_Renderer* renderer, bool press = false);
    ~Button_Image();
    bool checkpress();
    void reset();
    bool loadImage(SDL_Renderer* renderer);
    void handleEvent(const SDL_Event& e);
    void onClick();
    bool isInside(int x, int y) const;
    void print_Image(SDL_Renderer* renderer);
protected:
    std::string path;
    int x, y;
    SDL_Rect rect;
    SDL_Texture* texture;
    bool press;
};

#endif  // IMPORTANT_H