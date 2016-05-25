#include "chip8.h"

Chip8::Chip8() : 
    m_opcode(0x0000),
    m_address_register(0x0000),
    m_stack_pointer(0x00),
    m_program_counter(0x0200),
    m_delay_timer(0x0000),
    m_sound_timer(0x0000) {

    int i = 0;

    for (i = 0; i < MEMORY_SIZE; ++i) {
        m_memory[i] = 0x0;
    } 

    for (i = 0; i < REGISTERS; ++i) {
        m_registers[i] = 0x0;
    }

    for (i = 0; i < STACK_SIZE; ++i) {
        m_stack[i] = 0x0;
    }

    for (i = 0; i < SCREEN_WIDTH*SCREEN_HEIGHT; ++i) {
        m_screen_buffer[i] = 0x0;
    }
}

Chip8::~Chip8 () {
}
