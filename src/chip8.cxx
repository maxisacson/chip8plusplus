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

    // Clear the keypad
    for (i = 0; i < KEYPAD_SIZE; ++i) {
        m_keypad[i] = 0x0;
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
    if (!rom) {
        std::cerr << "--- Warning: Invalid rom file '" << rompath << "'" << std::endl;
        throw -3;
    }
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

    uint8_t  first_nibble  = (m_opcode & 0xf000) >> 12;
    uint8_t  second_nibble = (m_opcode & 0x0f00) >>  8;
    uint8_t  third_nibble  = (m_opcode & 0x00f0) >>  4;
    uint8_t  fourth_nibble = (m_opcode & 0x000f) >>  0;

    uint8_t  register_x    = second_nibble;
    uint8_t  register_y    = third_nibble;

    uint8_t  value_n       = (m_opcode & 0x000f);
    uint8_t  value_nn      = (m_opcode & 0x00ff);
    uint16_t value_nnn     = (m_opcode & 0x0fff);

    switch(first_nibble) {
        case 0x0:
            switch(fourth_nibble) {
                case 0x0:
                    // Clears the screen
                    for (int i = 0; i < SCREEN_WIDTH*SCREEN_HEIGHT; ++i) {
                        m_screen_buffer[i] = 0;
                    }
                    break;
                case 0xe:
                    // Returns from subroutine
                    m_program_counter = m_stack[m_stack_pointer--];
                    break;
                default:
                    std::cerr << "--- Warning: We should never be in this state. " <<
                        std::hex << m_opcode << std::endl;
                    throw -1;
                    break;
            }
            break;
        case 0x1:
            // Jumps to address NNN
            m_program_counter = value_nnn;
            break;
        case 0x2:
            // Calls subroutine at NNN
            m_stack[++m_stack_pointer] = m_program_counter;
            m_program_counter = value_nnn;
            break;
        case 0x3:
            // Skips the next instruction if VX equals NN
            if (m_registers[register_x] == value_nn) {
                m_program_counter += 2;
            }
            break;
        case 0x4:
            // Skips the next instruction if VX doesn't equal NN
            if (m_registers[register_x] != value_nn) {
                m_program_counter += 2;
            }
        case 0x6:
            // Sets VX to NN
            m_registers[register_x] = value_nn;
            break;
        case 0x7:
            // Adds NN to VX
            m_registers[register_x] += value_nnn;
            break;
        case 0x8:
            switch(fourth_nibble) {
                case 0x0:
                    // Sets VX to the value of VY
                    m_registers[register_x] = m_registers[register_y];
                    break;
                default:
                    std::cerr << "--- Warning: We should never be in this state: " <<
                        std::hex << m_opcode << std::endl;
                    throw -1;
                    break;
            }
            break;
        case 0x9:
            // Skips the next instruction if VX doesn't equal VY
            if (m_registers[register_x] != m_registers[register_y]) {
                m_program_counter += 2;
            }
            break;
        case 0xa:
            // Sets I to the address NNN
            m_address_register = value_nnn;
            break;
        case 0xc: {
                // Sets VX to the result of a bitwise and operation on a random number and NN
                // TODO: Implement a proper PRNG.
                uint8_t rnd = 0x3;
                m_registers[register_x] = rnd & value_nn;
                break;
            }
        case 0xd:
            // Draw to screen. Procedure is described in draw_to_screen method.
            draw_to_screen(register_x, register_y, value_n);
            break;
        case 0xe:
            switch( (third_nibble << 4) | fourth_nibble ) {
                case 0x9e:
                    // Skips the next instruction if the key stored in VX is pressed.
                    if ( m_keypad[ m_registers[register_x] ] == 0x1 ) {
                        m_program_counter += 2;
                    }
                    break;
                case 0xa1:
                    // Skips the next instruction if the key stored in VX isn't pressed.
                    if ( m_keypad[ m_registers[register_x] ] != 0x1 ) {
                        m_program_counter += 2;
                    }
                    break;
                default:
                    std::cerr << "--- Warning: We should never be in this state: " <<
                        std::hex << m_opcode << std::endl;
                    throw -1;
                    break;
            }
            break;
        case 0xf:
            switch( (third_nibble << 4) | fourth_nibble ) {
                case 0x07:
                    // Sets VX to the value of the delay timer
                    m_registers[register_y] = m_delay_timer;
                    break;
                case 0x15:
                    // Sets the delay timer to VX
                    m_delay_timer = m_registers[register_x];
                    break;
                case 0x1e:
                    // Adds VX to I
                    m_address_register += m_registers[register_x];
                    break;
                case 0x65:
                    // Fills V0 to VX (including VX) with values from memory starting at address I
                    for (uint16_t offset = 0; offset <= register_x; ++offset) {
                        m_registers[offset] = m_memory[m_address_register + offset];
                    }
                    break;
                default:
                    std::cerr << "--- Warning: We should never be in this state. " <<
                        std::hex << m_opcode << std::endl;
                    throw -1;
                    break;
            }
            break;
        default:
            std::cerr << "--- Warning: Unrecognized instruction: " << std::hex << m_opcode << std::endl;
            throw -16;
            break;
    }
}

void Chip8::draw_to_screen(uint8_t reg_x, uint8_t reg_y, uint8_t height) {
    // draw_to_screen is responsible of drawing to the screen buffer

    // Sprites stored in memory at location in index register (I), 8bits wide. Wraps around the
    // screen. If when drawn, clears a pixel, register VF is set to 1 otherwise it is zero. All
    // drawing is XOR drawing (i.e. it toggles the screen pixels). Sprites are drawn starting at
    // position VX, VY. N is the number of 8bit rows that need to be drawn. If N is greater than 1,
    // second line continues at position VX, VY+1, and so on.

    uint16_t start_x = m_registers[reg_x];
    uint16_t start_y = m_registers[reg_y];

    uint8_t  sprite        = 0x0;
    uint8_t  pixel         = 0x0;
    uint8_t  current_pixel = 0x0;
    uint8_t  x_offset_bits = 0x7;
    uint16_t pos_x         = 0x0;
    uint16_t pos_y         = 0x0;

    m_registers[0xf] = 0x0;

    for (int line_y = 0; line_y < height; ++line_y) {
        sprite = m_memory[m_address_register + line_y];
        pos_y = start_y + line_y;
        for (int line_x = 0; line_x < 8; ++line_x, --x_offset_bits) {
            pixel = (sprite >> x_offset_bits) & 0x1;
            pos_x = start_x + line_x;
            current_pixel = m_screen_buffer[pos_x + pos_y*SCREEN_WIDTH];
            if (current_pixel == 0x1 && pixel == 0x1) {
                // Collision detected. Set VF to 1
                m_registers[0xf] = 0x1;
            }
            m_screen_buffer[pos_x + pos_y*SCREEN_WIDTH] = (current_pixel ^ pixel) & 0x1;
        }
    }
}

void Chip8::decrement_timers() {
    if (m_delay_timer > 0) {
        --m_delay_timer;
    }

    if (m_sound_timer > 0) {
        --m_sound_timer;
    }
}

void Chip8::dump_screen_buffer() {
    // dump_screen_buffer dumps the screen buffer to stdout. Can be used as a rudimentary display.

    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (m_screen_buffer[x + y*SCREEN_WIDTH] == 1) {
                std::cout << "@";
            } else {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
}

void Chip8::dump_memory() {
    // dump_memory dumps all 4096 bytes of memory to stdout

    for (int i = 0; i < MEMORY_SIZE; ++i) {
        std::cout << m_memory[i];
    }
    std::cout << std::endl;
}
