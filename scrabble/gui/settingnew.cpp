#include "include.h"
#include "GUI.h"
#include "settingnew.h"
Image* setting_image;
Image * setting_function;
Button_Text* decrease_1;
Button_Text* decrease_2;
Button_Text* decrease_3;
void init_settings_UI() {
    setting_image = new Image("assets/images/Setting.png", WIDTH/3-50, HEIGHT*(9/10), renderer);
    setting_function = new Image("assets/images/edit.png", WIDTH/3-350, HEIGHT*(3/4), renderer);
    decrease_1 = new Button_Text("<", WIDTH/2+100, HEIGHT/4-60, renderer, green, false);
    decrease_2 = new Button_Text("<", WIDTH/2+100, HEIGHT/3+5, renderer, green, false);
    decrease_3 = new Button_Text("<", WIDTH/2+100, HEIGHT/2-25, renderer, green, false);
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
    while (!quit)
    {
        SDL_RenderClear(renderer);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                // Handle mouse button down events here
            }
        }
        // Render settings UI elements here
        // For example, draw buttons, sliders, etc.
        setting_image->print_Image(renderer);
        setting_function->print_Image(renderer);
        decrease_1->print_Text(renderer);
        decrease_2->print_Text(renderer);
        decrease_3->print_Text(renderer);
        // Update the screen
        SDL_RenderPresent(renderer);
    }
    
}
