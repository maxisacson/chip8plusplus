#ifndef CHIP8_H_
#define CHIP8_H_

#define MEMORY_SIZE 0x1000
#define REGISTERS 16
#define STACK_SIZE 16
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

#include <cstdint>

class Chip8 {
    public:
        Chip8();
        virtual ~Chip8();

    private:

        void fetch_instruction();
        void run_instruction();

        uint16_t m_opcode;

        uint8_t m_memory[MEMORY_SIZE];
        uint8_t m_registers[REGISTERS];
        
        uint16_t m_address_register;

        uint16_t m_stack[STACK_SIZE];
        
        uint8_t m_stack_pointer;

        uint16_t m_program_counter;

        uint16_t m_delay_timer;
        uint16_t m_sound_timer;

        uint8_t m_screen_buffer[SCREEN_HEIGHT*SCREEN_WIDTH];
};

#endif /* CHIP8_H_ */
