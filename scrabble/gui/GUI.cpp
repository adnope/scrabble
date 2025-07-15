#include "include.h"
using namespace std;
int choose=0;
struct GUI{
    void run_GUI_first(){
        while(!quit) {           
            SDL_RenderClear(renderer);
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                    
                    // Check if the start button is clicked
                    startbutton->handleEvent(e);
                    quitbutton->handleEvent(e);
                    settings->handleEvent(e);
                    if(startbutton->checkpress()){
                        choose = 1; // Start the game
                    } else if(quitbutton->checkpress()){
                        choose = -1; // Quit the game
                    } else if(settings->checkpress()){
                        choose = 2; // Open settings
                    }
                }
            }
            print_Welcome();
            if(choose==1) {
                // Start the game
                // Call the function to start the game here
                cout<< "Starting the game..." << endl;
            } else if(choose==2) {
                // Open settings
                // Call the function to open settings here
                cout<< "Opening settings..." << endl;
            } else if(choose==-1) {
                // Quit the game
                quit = true;
            }
            SDL_RenderPresent(renderer);
        }
    }
};