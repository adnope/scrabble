#include "include.h"
using namespace std;

SDL_Renderer* renderer = nullptr;
SDL_Window* window = nullptr;
bool quit = false;
SDL_Event event;
int choose=0;
void initRenderer() {
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        cerr << "Could not create renderer! SDL_Error: " << SDL_GetError() << endl;
        exit(1);
    }
}

// ===== CLASS letter =====
class letter {
public:
    // Constructor nhận x, y thay vì SDL_Rect
    letter(const string& s, SDL_Renderer* renderer, int x, int y, TTF_Font* font, SDL_Color color)
        : s(s), font(font)
    {
        SDL_Surface* surface = TTF_RenderText_Solid(font, s.c_str(), color);
        if (!surface) {
            cerr << "Surface error: " << TTF_GetError() << endl;
            texture = nullptr;
            return;
        }
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            cerr << "Texture error: " << SDL_GetError() << endl;
        }
        // Tự động lấy kích thước từ surface
        rect.x = x;
        rect.y = y;
        rect.w = surface->w;
        rect.h = surface->h;
        SDL_FreeSurface(surface);
    }

    virtual ~letter() {
        if (texture) SDL_DestroyTexture(texture);
    }

    virtual void print(SDL_Renderer* renderer) {
        if (texture)
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }

protected:
    string s;
    SDL_Texture* texture = nullptr;
    SDL_Rect rect;
    TTF_Font* font;
};

// ===== CLASS welcomeletter (kế thừa letter) =====
class welcomeletter : public letter {
public:
    using letter::letter;
};
class START: public letter{
    using letter::letter;
    public:
        // Add a callback function pointer or std::function for button action
        void onClick(){
            choose=1;
        };

        // Check if a point is inside the button
        bool isInside(int x, int y) const {
            return x >= rect.x && x <= rect.x + rect.w &&
                   y >= rect.y && y <= rect.y + rect.h;
        }

        // Handle SDL mouse event
        void handleEvent(const SDL_Event& e) {
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mx = e.button.x;
                int my = e.button.y;
                if (isInside(mx, my)) {
                    onClick();
                }
            }
        }
};
class quitletter :public letter{
    using letter::letter;
    public:
        void onClick(){
            choose=-1;
        };

        // Check if a point is inside the button
        bool isInside(int x, int y) const {
            return x >= rect.x && x <= rect.x + rect.w &&
                   y >= rect.y && y <= rect.y + rect.h;
        }

        // Handle SDL mouse event
        void handleEvent(const SDL_Event& e) {
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mx = e.button.x;
                int my = e.button.y;
                if (isInside(mx, my)) {
                    onClick();
                }
            }
        }
};
// ===== MAIN =====
int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL Init failed: " << SDL_GetError() << endl;
        return 1;
    }
    if (TTF_Init() == -1) {
        cerr << "TTF Init failed: " << TTF_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    // Create window
    window = SDL_CreateWindow("Scrabble Welcome", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        cerr << "Window creation failed: " << SDL_GetError() << endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Create renderer
    initRenderer();

    // Load font
    TTF_Font* font = TTF_OpenFont("Aptos.ttf", 32);
    if (!font) {
        cerr << "Font load failed: " << TTF_GetError() << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Tọa độ welcome có thể thay đổi dễ dàng
    int windowWidth = 0, windowHeight = 0;
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        cerr << "SDL_GetCurrentDisplayMode failed: " << SDL_GetError() << endl;
        windowWidth = 800;
        windowHeight = 600;
    } else {
        //cout<<displayMode.w<<" "<<displayMode.h<<endl;
        windowWidth = displayMode.w-325;
        windowHeight = displayMode.h;
    }
    // Khởi tạo welcome letter với x, y
    int welcomeX = windowWidth / 2;
    int welcomeY = windowHeight / 10;
    SDL_Color welcomeColor = {255, 255, 255, 255};
    welcomeletter* welcome = new welcomeletter("This is Scrabble Game", renderer, welcomeX, welcomeY, font, welcomeColor);
    //khởi tạo start
    int startX = windowWidth / 2 + 100;
    int startY = windowHeight / 2 - 100;
    SDL_Color startColor = {0, 255, 0, 255};
    START* startButton = new START("START", renderer, startX, startY, font, startColor);
    //khởi tạo quit
    int quitX = windowWidth / 2 + 110;
    int quitY = windowHeight - 200;
    SDL_Color quitColor = {255, 0, 0, 255};
    quitletter* quitButton = new quitletter("QUIT", renderer, quitX, quitY, font, quitColor);
    // Main loop
    while (!quit) {
        choose=0;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                quit = true;
            startButton->handleEvent(event);
            quitButton->handleEvent(event);
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // In welcome text
        welcome->print(renderer);
        startButton->print(renderer);
        quitButton->print(renderer);
        if(choose==1){
            cout<<123<<endl;
        }
        else if(choose==-1){
            quit=true;
        }
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    delete welcome;
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
