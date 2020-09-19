#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
// #include <SDL2/SDL_rect.h>

#include "chip8.h"

const int32_t PX_WIDTH = 16;
const int32_t PX_HEIGHT = 16;

void draw_screen_buffer(SDL_Renderer *ren, uint8_t buffer[]) {
    for (int32_t y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int32_t x = 0; x < SCREEN_WIDTH; ++x) {
            if (buffer[x + y*SCREEN_WIDTH] == 1) {
                SDL_Rect rect = {x*PX_WIDTH, y*PX_HEIGHT, PX_WIDTH, PX_HEIGHT};
                SDL_RenderFillRect(ren, &rect);
            } else {

            }
        }
    }

    SDL_RenderPresent(ren);
}

void clear_screen(SDL_Renderer *ren) {
    SDL_RenderClear(ren);
}

int main(int argc, char** argv) {
    // Initialize SDL and stuff
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // SDL_Window *win = SDL_CreateWindow("Chip8++", 100, 100, 1024, 512, SDL_WINDOW_SHOWN);
    SDL_Window *win = SDL_CreateWindow("Chip8++", 100, 100, PX_WIDTH*SCREEN_WIDTH, PX_HEIGHT*SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (win == nullptr) {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == nullptr) {
        SDL_DestroyWindow(win);
        std::cerr << "SDL_Renderer error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    Chip8 cpu;

    // uint32_t last_time = 0, current_time;

    const int32_t FPS = 60;

    int32_t start = 0;
    int32_t time_left = 0;

    try {
        // cpu.load_rom("roms/INVADERS");
        // cpu.load_rom("roms/PONG");
        cpu.load_rom("roms/PONG2");

        bool done = false;
        while (!done) {
            // TODO: Should the timers be decremented before of after running a instruction?

            start = SDL_GetTicks();

            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    done = true;
                    break;
                } else if (e.type == SDL_KEYDOWN) {
                    switch(e.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            done = true;
                            break;
                        default:
                            break;
                    }
                }
            }

            cpu.decrement_timers();
            cpu.fetch_instruction();
            cpu.run_instruction();
            // cpu.dump_screen_buffer();

            clear_screen(ren);
            draw_screen_buffer(ren, cpu.m_screen_buffer);

            time_left = (1000/FPS) - (SDL_GetTicks() - start);

            if ( time_left > 0 ) {
                SDL_Delay(time_left);
            }
        }

    } catch (int e) {
        std::cerr << "--- Exception: " << e << std::endl;
        SDL_DestroyWindow(win);
        SDL_DestroyRenderer(ren);
        SDL_Quit();

        return -1;
    }

    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(ren);
    SDL_Quit();

    return 0;
}
