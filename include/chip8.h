#ifndef CHIP8_H
#define CHIP8_H

#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#define MAX_WIDTH 64
#define MAX_HEIGHT 32
#define MAX_KEYS 16
#define MAX_RAM 4096
#define MAX_REGISTERS 16

#define FONT_START_ADDR 0x0
#define SP_START_ADDR 0x50
#define PC_START_ADDR_DEFAULT 0x200
#define CLOCK_SPEED_DEFAULT 500

#define KEY_UP 0
#define KEY_DOWN 1
#define KEY_RELEASED 2

typedef struct CHIP8
{
    // Represents 4096 bytes (4KB) of addressable memory.
    unsigned char RAM[MAX_RAM];

    // Represents 16 general-purpose 8-bit registers (V0-VF).
    unsigned char V[MAX_REGISTERS];

    // Program counter, stack pointer, and index 16-bit registers.
    unsigned int PC, SP, I;

    // Delay timer and sound timer 8-bit registers.
    unsigned char DT, ST;

    /* A monochrome display of 64x32 pixels.
    A pixel can be either only on or off, no color. */
    bool display[MAX_HEIGHT][MAX_WIDTH];

    // Represents keys 0-F and if they are down, up, or released.
    int keypad[MAX_KEYS];

    /* The emulator can be ran in SUPER CHIP-8 mode by default,
    or legacy mode compatible with the original COSMAC VIP. */
    bool legacy_mode;

    /* Where the emulator begins processing. Some programs,
    such as those written for the ETI-600, begin at different addresses. */
    unsigned int pc_start_addr;

    // These are used for handling clock and timer speed.
    int clock_speed;
    int timer_max_cum;
    int cpu_max_cum;
    long cpu_cum;
    long sound_cum;
    long delay_cum;
    long total_cycle_time;
    struct timeval cur_cycle_start;
    struct timeval prev_cycle_start;

    // Used to signal to main when to update the display and produce sound.
    bool display_updated;
    bool beep;
} CHIP8;

// Set some things to useful default values.
void chip8_init(CHIP8 *chip8, bool legacy_mode, int clock_speed, int pc_start_addr);

// Reset the machine.
void chip8_reset(CHIP8 *chip8);

/* Load hexadecimal font into memory.
Each hex character is represented by 5 bytes in memory
with each bit representing a pixel. */
void chip8_load_font(CHIP8 *chip8);

/* Loads a given ROM into memory starting at address PC_START_ADDR.
>=PC_START_ADDR is where user data is stored.
Everything before that is system. */
bool chip8_load_rom(CHIP8 *chip8, char *filename);

// Fetches, decodes, and executes the next instruction.
void chip8_execute(CHIP8 *chip8);

// Decrements delay and sound timers at a frequency of 60Hz.
void chip8_handle_timers(CHIP8 *chip8);

// Updates the total cycle time since last call.
void chip8_update_elapsed_time(CHIP8 *chip8);

// Clears the keypad by setting all keys to up.
void chip8_reset_keypad(CHIP8 *chip8);

// Resets any key that was released to KEY_UP.
void chip8_reset_released_keys(CHIP8 *chip8);

// Clears the display by setting all pixels to off.
void chip8_reset_display(CHIP8 *chip8);

// Clears the RAM.
void chip8_reset_RAM(CHIP8 *chip8);

// Clears all registers.
void chip8_reset_registers(CHIP8 *chip8);

// Loads an instruction into PC_START_ADDR.
void chip8_load_instr(CHIP8 *chip8, unsigned int instr);

// Draws n bytes starting at address I onto the display at coordinates (Vx, Vy).
void chip8_draw(CHIP8 *chip8, unsigned char x, unsigned char y, unsigned char n);

// Waits for a key to be released then stores that key in Vx.
void chip8_wait_key(CHIP8 *chip8, unsigned char x);

// Dumps the current contents of RAM to a specified file.
bool chip8_dump_RAM(CHIP8 *chip8, char *filename);

#endif
