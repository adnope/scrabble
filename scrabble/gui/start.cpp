#include "include.h"
#include "GUI.h"
#include "start.h"
using namespace std;
Image* chooseplayer;
Button_Image* two;
Button_Image* three;
Button_Image* four;
Button_Image* home_next;
void init_start(){
    chooseplayer = new Image("assets/images/chooseplayer.png",WIDTH/10-210,HEIGHT/15,renderer);
    two   = new Button_Image("assets/images/2.png", WIDTH/5,HEIGHT/2,renderer);
    home_next = new Button_Image("assets/images/home.png", 5, 5, renderer);
}
void print_start(SDL_Event &e){
    init_start();
    while (!quit)
    {
        SDL_RenderClear(renderer);
        two->reset();
        home_next->reset();
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if(e.type == SDL_MOUSEBUTTONDOWN){
                home_next->handleEvent(e);
            }
        }
        home_next->print_Image(renderer);
        chooseplayer->print_Image(renderer);
        two->print_Image(renderer);
        if(home_next->checkpress()){
            SDL_RenderClear(renderer);
            return;
        }
        SDL_RenderPresent(renderer);
        /* code */
    }
    
}
