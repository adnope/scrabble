#include "include.h"
#include "welcome.h"
using namespace std;
Image *background =new Image("assets/images/MainmenuScene.png", 0, 0, renderer);
Button_Image *startbutton= new Button_Image("assets/images/startbutton.png", 100, 100, renderer);
Button_Image *quitbutton= new Button_Image("assets/images/quitgamebutton.png", 200, 200, renderer);
Button_Image* settings=new Button_Image("assets/images/settingicon.png", 300, 300, renderer);
void print_Welcome() {
    SDL_RenderClear(renderer);
    background->print_Image(renderer);
    startbutton->print_Image(renderer);
    quitbutton->print_Image(renderer);
    settings->print_Image(renderer);
    SDL_RenderPresent(renderer);
}