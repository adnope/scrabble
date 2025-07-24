#include "GUI.h"
#include "include.h"
#include "welcome.h"
#include "settingnew.h"
#include "start.h"
using namespace std;
int choose = 0;
SDL_Event e;
void GUI::run_GUI_first() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << '\n';
        return ;
    }
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << '\n';
        SDL_Quit();
        return ;
    }
    // std::cout << "Before TTF_Init" << std::endl;
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << '\n';
        IMG_Quit();
        SDL_Quit();
        return ;
    }
    //std::cout << "After TTF_Init" << std::endl;
    // Set background color: R, G, B, A (0-255)
    SDL_SetRenderDrawColor(renderer, 235, 252, 255, 255);  // Ví dụ: màu xanh da trời

    // Xóa màn hình bằng màu đã set (fill full screen)
    SDL_RenderClear(renderer);
    //cout<<HEIGHT<<" "<<WIDTH<<endl;
    init_font();
    while (!quit) {
        //std::cout<<123<<std::endl;
        //SDL_RenderSetLogicalSize(renderer, 1280, 720);
        choose=0;
        startbutton->reset();
        quitbutton->reset();
        settings->reset();
        SDL_RenderClear(renderer);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                startbutton->handleEvent(e);
                quitbutton->handleEvent(e);
                settings->handleEvent(e);
                if (startbutton->checkpress()) {
                    choose = 1;
                } else if (quitbutton->checkpress()) {
                    choose = -1;
                } else if (settings->checkpress()) {
                    choose = 2;
                }
            }
        }
        print_Welcome();
        if (choose == 1) {
            print_start(e);
        } else if (choose == 2) {
            print_setting(e);
            // TODO: Implement settings menu functionality here if needed
        } else if (choose == -1) {
            std::cout << choose << std::endl;
            quit = true;
        }
        SDL_RenderPresent(renderer);
    }
    TTF_CloseFont(font_Jersey15_Regular);
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return ;
}
