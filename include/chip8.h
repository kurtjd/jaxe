#ifndef CHIP8_H
#define CHIP8_H

#include "constants.h"

typedef struct Machine
{
    // Represents 4096 bytes (4KB) of addressable memory.
    unsigned char RAM[MAX_RAM];

    // Represents 16 general-purpose 8-bit registers (V0-VF).
    unsigned char V[MAX_REGISTERS];

    // Delay timer, sound timer and stack pointer 8-bit registers.
    unsigned char DT, ST, SP;

    // Program counter and index 16-bit registers.
    unsigned int PC, I;

    // The call stack. CHIP-8 allows a max of 16 nested calls.
    unsigned int stack[MAX_STACK_LEVELS];

    /* A monochrome display of 64x32 pixels.
    A pixel can be either only on or off, no color. */
    bool display[MAX_HEIGHT][MAX_WIDTH];

    // Represents keys 0-F and if they are pressed or not.
    bool keypad[MAX_KEYS];
} Machine;

void machine_init(Machine *machine);
void machine_load_font(Machine *machine);
bool machine_load_rom(Machine *machine, char *filename);
void machine_execute(Machine *machine);
void machine_beep();
void machine_handle_timers(Machine *machine);

#endif