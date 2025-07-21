#include "include.h"
#include "GUI.h"
#include "settingnew.h"
using namespace std;
Image* setting_image;
Image * setting_function;
Button_Text* decrease_1;
Button_Text* decrease_2;
Button_Text* decrease_3;
Button_Text* increase_1;
Button_Text* increase_2;
Button_Text* increase_3;
Button_Image* home;
Button_Text* res_1900x1200;
Button_Text* res_1280x720;
Image* save;
Button_Text* Vsync_YES;
Button_Text* Vsync_NO;
void init_settings_UI() {
    setting_image = new Image("assets/images/Setting.png", WIDTH/3-50, HEIGHT*(9/10), renderer);
    setting_function = new Image("assets/images/edit.png", WIDTH/3-350, HEIGHT*(3/4), renderer);
    decrease_1 = new Button_Text("<", WIDTH/2+100, HEIGHT/4-60, renderer, green, false,font_aptos);
    decrease_2 = new Button_Text("<", WIDTH/2+100, HEIGHT/3+5, renderer, green, false, font_aptos);
    decrease_3 = new Button_Text("<", WIDTH/2+100, HEIGHT/2-25, renderer, green, false,font_aptos);
    increase_1 =new Button_Text(">", WIDTH/2+500, HEIGHT/4-60, renderer, green, false,font_aptos);
    increase_2 =new Button_Text(">", WIDTH/2+500, HEIGHT/3+5, renderer, green, false,font_aptos);
    increase_3 =new Button_Text(">", WIDTH/2+500, HEIGHT/2-25, renderer, green, false,font_aptos);
    home       =new Button_Image("assets/images/home.png",5,5,renderer);
    res_1280x720 = new Button_Text("1280x720", WIDTH/2+170, HEIGHT/4-30, renderer, green, false,font_aptos_small);
    res_1900x1200 = new Button_Text("1900x1200", WIDTH/2+170, HEIGHT/4-30, renderer, green, false,font_aptos_small);
    Vsync_YES = new Button_Text("YES", WIDTH/2+170, HEIGHT/3+30, renderer, green, false,font_aptos_small);
    Vsync_NO = new Button_Text("NO", WIDTH/2+170, HEIGHT/3+30, renderer, green, false,font_aptos_small);
    save = new Image("assets/images/savesetting.png", WIDTH/2-250, HEIGHT-200, renderer);
    // Add
}
void print_setting (SDL_Event &e) {
    if (!font_Jersey15_Regular) {
        std::cerr << "font_Jersey15_Regular is NULL!" << std::endl;
    }
    if (!font_aptos) {
        std::cerr << "font_Aptos is NULL!" << std::endl;
        //return;
    }
    init_settings_UI();
    int choice_1=0;
    int choice_2=0;
    int choice_3=0;
    while (!quit)
    {
        //reset button
        decrease_1->reset();
        decrease_2->reset();
        decrease_3->reset();
        increase_1->reset();
        increase_2->reset();
        increase_3->reset();
        home->reset();
        SDL_RenderClear(renderer);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                // Handle mouse button down events here
                decrease_1->handleEvent(e);
                decrease_2->handleEvent(e);
                decrease_3->handleEvent(e);
                increase_1->handleEvent(e);
                increase_2->handleEvent(e);
                increase_3->handleEvent(e);
                home->handleEvent(e);
            }
        }
        // Render settings UI elements here
        // For example, draw buttons, sliders, etc.
        setting_image->print_Image(renderer);
        setting_function->print_Image(renderer);
        decrease_1->print_Text(renderer);
        decrease_2->print_Text(renderer);
        decrease_3->print_Text(renderer);
        increase_1->print_Text(renderer);
        increase_2->print_Text(renderer);
        increase_3->print_Text(renderer);
        if(choice_1 == 0) {
            res_1900x1200->print_Text(renderer);
        } else {
            res_1280x720->print_Text(renderer);
        }
        //res_1900x1200->print_Text(renderer);
        home->print_Image(renderer);
        save->print_Image(renderer);
        if(decrease_1 ->checkpress()) {
            choice_1--;
            choice_1%=2; // Prevent going below 0
            //cout<<"choice 1:"<<choice_1<<endl;
        }
        if(decrease_2 ->checkpress()) {
            choice_2--;
            choice_2%=3; // Prevent going below 0
            cout<<"choice 2:"<<choice_2<<endl;
        }
        if(decrease_3 ->checkpress()) {
            choice_3--;
            choice_3%=3; // Prevent going below 0
            cout<<"choice 3:"<<choice_3<<endl;
        }
        if(increase_1 ->checkpress()) {
            choice_1++;
            choice_1%=2; // Prevent going above 2
            //cout<<"choice 1:"<<choice_1<<endl;
        }
        if(increase_2 ->checkpress()) { 
            choice_2++;
            choice_2%=3; // Prevent going above 2
            cout<<"choice 2:"<<choice_2<<endl;
        }
        if(increase_3 ->checkpress()) { 
            choice_3++;
            choice_3%=3; // Prevent going above 2
            cout<<"choice 3:"<<choice_3<<endl;
        }
        if (home->checkpress())
        {
            SDL_RenderClear((renderer));
            return;
            /* code */
        }
        
        // Update the screen
        SDL_RenderPresent(renderer);
    }
    
}