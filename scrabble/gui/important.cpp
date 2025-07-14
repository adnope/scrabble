#include "include.h"
using namespace std;
const int FONT_SIZE = 32;
TTF_Font *font_Jersey15_Regular = TTF_OpenFont("assets/fonts/Jersey15-Regular.ttf",FONT_SIZE);
TTF_Font *font_aptos = TTF_OpenFont("assets/fonts/Aptos.ttf",FONT_SIZE);
SDL_Color white = {255, 255, 255, 255};
SDL_Color black = {0, 0, 0, 255};
SDL_Color red = {255, 0, 0, 255};
SDL_Color green = {0, 255, 0, 255};
SDL_Color blue = {0, 0, 255, 255};
SDL_Color yellow = {255, 255, 0, 255};
SDL_Color orange = {255, 165, 0, 255};
SDL_Color purple = {128, 0, 128, 255};
SDL_Color gray = {128, 128, 128, 255};
SDL_Color light_gray = {211, 211, 211, 255};
SDL_Color brown = {139, 69, 19, 255};
SDL_Color cyan = {0, 255, 255, 255};
SDL_Color magenta = {255, 0, 255, 255};
SDL_Color pink = {255, 192, 203, 255};
SDL_Color gold = {255, 215, 0, 255};
SDL_Color dark_green = {0, 100, 0, 255};
class Text{
    public:
        Text(const string& s, int x, int y, SDL_Renderer* renderer, SDL_Color color = white)
            : s(s), x(x), y(y), texture(nullptr), color(color) {
            SDL_Surface* surface = TTF_RenderUTF8_Blended(font_Jersey15_Regular, s.c_str(), color);
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
        void print_Text(SDL_Renderer* renderer) {
            if (texture) {
                SDL_RenderCopy(renderer, texture, nullptr, &rect);
            }
        }
    protected:
        SDL_Color color;
        string s;
        int x,y; // position
        SDL_Rect rect;  //rectangle; 
        SDL_Texture* texture; // texture for rendering the Text
};
class Image{
    public:
        Image(string imagePath, int x, int y, SDL_Renderer* renderer) 
            : path(imagePath), x(x), y(y), texture(nullptr) {
            loadImage(renderer);
        }
        
        ~Image() {
            if (texture) {
                SDL_DestroyTexture(texture);
            }
        }
        
        bool loadImage(SDL_Renderer* renderer) {
            // Load the image surface from file
            SDL_Surface* surface = IMG_Load(path.c_str());
            if (!surface) {
                printf("Failed to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
                return false;
            }
            
            // Create texture from surface
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (!texture) {
                printf("Failed to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
                SDL_FreeSurface(surface);
                return false;
            }
            
            // Set rectangle dimensions based on image size
            rect.x = x;
            rect.y = y;
            rect.w = surface->w;
            rect.h = surface->h;
            
            // Free the surface as we no longer need it
            SDL_FreeSurface(surface);
            return true;
        }
        
        void print_Image(SDL_Renderer* renderer) {
            if (texture) {
                SDL_RenderCopy(renderer, texture, nullptr, &rect);
            }
        }
        
        bool isLoaded() const { return texture != nullptr; }
        
    protected:
        string path;
        int x, y; // position
        SDL_Rect rect;
        SDL_Texture* texture;
};
class Button_Text{
    public:
        void onClick(){
            press=true;
        };

        // Check if a point is inside the button
        bool isInside(int x, int y) const {
            return x >= rect.x && x <= rect.x + rect.w &&
                   y >= rect.y && y <= rect.y + rect.h;
        }
        Button_Text(const string& s, int x, int y, SDL_Renderer* renderer, SDL_Color color = white)
            : s(s), x(x), y(y), texture(nullptr), color(color) {
            SDL_Surface* surface = TTF_RenderUTF8_Blended(font_Jersey15_Regular, s.c_str(), color);
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
        // create a button with text base on event mouse click
        void handleEvent(const SDL_Event& e) {
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mx = e.button.x;
                int my = e.button.y;
                if (isInside(mx, my)) {
                    onClick();
                }
            }
        }

    protected:
        string s;
        int x,y;
        SDL_Rect rect;
        SDL_Texture* texture;
        SDL_Color color;
        bool press;
};
class Button_Image{
    public:
        void onClick(){
            press=true;
        };

        // Check if a point is inside the button
        bool isInside(int x, int y) const {
            return x >= rect.x && x <= rect.x + rect.w &&
                   y >= rect.y && y <= rect.y + rect.h;
        }
        Button_Image(string imagePath, int x, int y, SDL_Renderer* renderer) 
            : path(imagePath), x(x), y(y), texture(nullptr) {
            loadImage(renderer);
        }

        ~Button_Image() {
            if (texture) {
                SDL_DestroyTexture(texture);
            }
        }
        
        bool loadImage(SDL_Renderer* renderer) {
            // Load the image surface from file
            SDL_Surface* surface = IMG_Load(path.c_str());
            if (!surface) {
                printf("Failed to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
                return false;
            }
            
            // Create texture from surface
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (!texture) {
                printf("Failed to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
                SDL_FreeSurface(surface);
                return false;
            }
            
            // Set rectangle dimensions based on image size
            rect.x = x;
            rect.y = y;
            rect.w = surface->w;
            rect.h = surface->h;
            
            // Free the surface as we no longer need it
            SDL_FreeSurface(surface);
            return true;
        }
        // create a button with text base on event mouse click
        void handleEvent(const SDL_Event& e) {
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mx = e.button.x;
                int my = e.button.y;
                if (isInside(mx, my)) {
                    onClick();
                }
            }
        }
    protected:
        string path;
        int x,y;
        SDL_Rect rect;
        SDL_Texture* texture;
        bool press;
};