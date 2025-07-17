#include "include.h"
#include "welcome.h"
using namespace std;
Image *background =new Image("assets/images/MainmenuScene.png", 0, 0, renderer);
Button_Image *startbutton= new Button_Image("assets/images/startbutton.png", WIDTH/2-250, HEIGHT/2+50, renderer);
Button_Image *quitbutton= new Button_Image("assets/images/quitgamebutton.png", WIDTH/2-250 , HEIGHT-300, renderer);
Button_Image* settings=new Button_Image("assets/images/settingicon.png", 10, 10, renderer);
void print_Welcome() {
    SDL_RenderClear(renderer);
    background->print_Image(renderer);
    startbutton->print_Image(renderer);
    quitbutton->print_Image(renderer);
    settings->print_Image(renderer);
    SDL_RenderPresent(renderer);
}