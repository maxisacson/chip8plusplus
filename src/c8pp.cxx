#include "chip8.h"

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

int main(int argc, char** argv) {
    // Initialize SDL and stuff
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Chip8++", 100, 100, 1024, 512, SDL_WINDOW_SHOWN);
    if (!win) {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!ren) {
        SDL_DestroyWindow(win);
        std::cerr << "SDL_Renderer error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    Chip8 cpu;

    unsigned int last_time = 0, current_time; 
    try {
        cpu.load_rom("roms/INVADERS");
        while (true) {
            // TODO: Should the timers be decremented before of after running a instruction?
           
            current_time = SDL_GetTicks();
            //std::cout << "Current time: " << current_time << std::endl; 
            //std::cout << "dt:           " << current_time-last_time << std::endl; 
            if ( (current_time - last_time) >= 1000/60 ) {
                cpu.decrement_timers();
                std::cout << 1000./(current_time - last_time) << std::endl;
                last_time = current_time;
            }
            cpu.fetch_instruction();
            cpu.run_instruction();
            //cpu.dump_screen_buffer();
        }
        
    } catch (int e) {
        std::cerr << "--- Exception: " << e << std::endl;
        SDL_DestroyWindow(win);
        SDL_Quit();

        return -1;
    }

    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
