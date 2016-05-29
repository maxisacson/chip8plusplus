#include "chip8.h"

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL error: " << SDL_GetError() << std::endl;
        return -2;
    }

    Chip8 cpu;

    unsigned int last_time = 0, current_time; 
    try {
        current_time = SDL_GetTicks();
        cpu.load_rom("roms/TETRIS");
        while (true) {
            // TODO: Should the timers be decremented before of after running a instruction?
           
            std::cout << "Current time: " << current_time << std::endl; 
            if ( (current_time - last_time)*6 >= 100 ) {
                cpu.decrement_timers();
                std::cout << 1./(current_time - last_time) << std::endl;
                last_time = current_time;
            }
            cpu.fetch_instruction();
            cpu.run_instruction();
            //cpu.dump_screen_buffer();
        }
        
    } catch (int e) {
        std::cerr << "--- Exception: " << e << std::endl;
        return -1;
    }

    return 0;
}
