#include "chip8.h"

#include <iostream>
#include <fstream>

Chip8::Chip8() : 
    m_opcode(0x0000),
    m_address_register(0x0000),
    m_stack_pointer(-1),
    // We initialize the program counter to 0x200 (=512), since addresses 0x000-0x1ff (0-511) is not
    // used (well, we store the font set there but no rom data should go before 0x200)
    m_program_counter(0x0200),
    m_delay_timer(0x0000),
    m_sound_timer(0x0000) {

    int i = 0;

    // Clear the memory
    for (i = 0; i < MEMORY_SIZE; ++i) {
        m_memory[i] = 0x0;
    } 

    // Clear all registers
    for (i = 0; i < REGISTERS; ++i) {
        m_registers[i] = 0x0;
    }

    // Clear the stack
    for (i = 0; i < STACK_SIZE; ++i) {
        m_stack[i] = 0x0;
    }

    // Clear the screen
    for (i = 0; i < SCREEN_WIDTH*SCREEN_HEIGHT; ++i) {
        m_screen_buffer[i] = 0x0;
    }
}

Chip8::~Chip8 () {
}

void Chip8::load_rom(const char* rompath) {
    // load_rom reads a rom file from disk, and dumps the binary data in memory starting at 0x200

    std::ifstream rom(rompath, std::ios::binary); 
    int offset = 0;
    while(rom) {
        char byte;
        rom.get(byte);
        if (rom) {
            m_memory[0x200 + offset] = (uint8_t)byte;
        }
        ++offset;
    }
}

void Chip8::fetch_instruction() {
    // fetch_instruction is responsible for fetching the current opcode from memory and increasing the
    // program counter to the next instruction.

    // Each opcode is 2 bytes long, so we fetch the byte in memory at address m_program_counter, shift
    // if left by 8 bits and take the bitwise or with the value at address m_program_counter + 1.
    m_opcode = (m_memory[m_program_counter] << 8) | m_memory[m_program_counter + 1];
    m_program_counter += 2;
}

void Chip8::run_instruction() {
    // run_instruction is responsible for running the instruction fetched by fetch_instruction.
    // It accomplishes this by deconstructin the instruction into 4 nibbles (1 nibble is 4 bits),
    // does a swich on the first nibble and then act accordingly.
    
    uint8_t first_nibble  = (m_opcode & 0xf000) >> 12;
    uint8_t second_nibble = (m_opcode & 0x0f00) >> 8;
    uint8_t third_nibble  = (m_opcode & 0x00f0) >> 4;
    uint8_t fourth_nibble = (m_opcode & 0x000f) >> 0;
    
    uint8_t register_x    = second_nibble;
    uint8_t register_y    = third_nibble;

    uint8_t value_n       = (m_opcode & 0x000f);
    uint8_t value_nn      = (m_opcode & 0x00ff);
    uint16_t value_nnn    = (m_opcode & 0x0fff);

    switch(first_nibble) { 
        case 0x2:
            // Calls subroutine at NNN
            m_stack[++m_stack_pointer] = value_nnn;
            break;
        case 0x7:
            // Adds NN to VX
            m_registers[register_x] += value_nnn;
            break;
        case 0xa:
            // Sets I to the address NNN
            m_address_register = value_nnn;
            break;
        default:
            std::cerr << "--- Warning: Unrecognized instruction: " << std::hex << m_opcode << std::endl;
            throw -16;
            break;
    }
}

void Chip8::dump_memory() {
    // dump_memory dumps all 4096 bytes of memory to stdout

    for (int i = 0; i < MEMORY_SIZE; ++i) {
        std::cout << m_memory[i];
    }
    std::cout << std::endl;
}
