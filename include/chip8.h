#ifndef CHIP8_H_
#define CHIP8_H_

#define MEMORY_SIZE   0x1000 // 4KB of memory
#define REGISTERS     16
#define STACK_SIZE    16
#define KEYPAD_SIZE   16
#define SCREEN_WIDTH  64
#define SCREEN_HEIGHT 32

#include <cstdint>

class Chip8 {
    public:
        Chip8();
        virtual ~Chip8();

        void load_rom(const char*);
        void fetch_instruction();
        void run_instruction();
        void draw_to_screen(uint8_t, uint8_t, uint8_t);
        void decrement_timers();

        void dump_screen_buffer();
        void dump_memory();

    private:
        // Each opcode is 2 bytes (16 bits) long
        uint16_t m_opcode;

        // The memory in the Chip8 consists of 4096 addresses each
        // capable of storing 1 byte (8 bits)
        uint8_t m_memory[MEMORY_SIZE];

        // The Chip8 has 16 1 byte (8 bits) registers
        uint8_t m_registers[REGISTERS];
       
        // The address (index) register is capable of storing memory addresses. The address space is 
        // 0x000-0xfff, so we need at least 12 bits for the encoding
        uint16_t m_address_register;

        // The Chip8 is capable of storing 16 addresses in the stack, used when running subroutines.
        // Since each entry in the stack is an address we need at least 12 bits to encode them
        uint16_t m_stack[STACK_SIZE];
       
        // The stack pointer points to the top of the stack. It should ALWAYS point to a valid address if the
        // stack is not empty, then it equals -1. To not fuck this up ALWAYS push the stack using
        //      
        //      m_stack[++m_stack_pointer] = some_address;
        //
        // and to pop from the stack ALWAYS use
        //
        //      some_address = m_stack[m_stack_pointer--];
        //
        // I.e. note the position of the increment/decrement operators. 
        int m_stack_pointer;

        // The program counter always points to the memory address where the NEXT instruction is
        // located. I.e., the program counter should ALWAYS be incremented by 2 (since each instruction
        // is 2 bytes long) after an instruction has been read
        // 
        //      opcode = m_memory[m_program_counter];
        //      m_program_counter += 2;
        //
        // to keep things internally consistent.
        uint16_t m_program_counter;

        // The Chip8 has 2 timers, the delay timer and the sound timer, if they are > 0 they should be
        // decremented by 1 at a rate of 60Hz, i.e. once every 1/60:th second.
        // TODO: implement this countdown.
        uint16_t m_delay_timer;
        uint16_t m_sound_timer;

        // The key pad
        // A 16 bit hexadecimal keypad
        //
        //    1 2 3 c
        //    4 5 6 d
        //    7 8 9 e
        //    a 0 b f
        //
        // The keypad table will store 1 when the key is pressed and 0 when it is released.
        uint8_t m_keypad[KEYPAD_SIZE];

        // The screen of the Chip8 is 64 pixels wide and 32 pixels height. Each pixel has 2 states: ON (1)
        // or OFF (0). When a pixel is on it is drawn, otherwise not.
        uint8_t m_screen_buffer[SCREEN_HEIGHT*SCREEN_WIDTH];
};

#endif /* CHIP8_H_ */
