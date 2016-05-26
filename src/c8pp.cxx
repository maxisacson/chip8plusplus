#include "chip8.h"

#include <iostream>

int main(int argc, char** argv) {
    Chip8 cpu;

    try {
        cpu.load_rom("roms/BRIX");
        while (true) {
            // TODO: Should the timers be decremented before of after running a instruction?
            cpu.decrement_timers();
            cpu.fetch_instruction();
            cpu.run_instruction();
            cpu.dump_screen_buffer();
        }
        
    } catch (int e) {
        std::cerr << "--- Exception: " << e << std::endl;
        return -1;
    }

    return 0;
}
