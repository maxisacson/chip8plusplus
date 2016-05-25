#include "chip8.h"

#include <iostream>

int main(int argc, char** argv) {
    Chip8 cpu;

    try {
        cpu.load_rom("roms/TETRIS");
        while (true) {
            cpu.fetch_instruction();
            cpu.run_instruction();
        }
        
    } catch (int e) {
        std::cerr << "--- Exception: " << e << std::endl;
        return -1;
    }

    return 0;
}
