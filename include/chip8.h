#ifndef CHIP8_H
#define CHIP8_H

#ifndef __LIBRETRO__
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#endif
#if defined(_MSC_VER) && _MSC_VER < 1800
#ifdef __LIBRETRO__
#include "libretro.h"
#else
#define bool int
#define true 1
#define false 0
#endif
#else
#include <stdbool.h>
#endif
#include <stdint.h>

#define ONE_SEC 1000000

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

#define NUM_KEYS 16
#define NUM_REGISTERS 16
#define NUM_USER_FLAGS 16
#define NUM_QUIRKS 10
#define NUM_BITPLANES 3
#define NUM_FONT_BYTES 80
#define NUM_BIG_FONT_BYTES 160

#define MAX_RAM 65536
#define MAX_FILEPATH_LEN 256
#define STACK_SIZE 16
#define AUDIO_BUF_SIZE 16

#define FONT_START_ADDR 0x0
#define BIG_FONT_START_ADDR (FONT_START_ADDR + NUM_FONT_BYTES)
#define SP_START_ADDR (BIG_FONT_START_ADDR + NUM_BIG_FONT_BYTES)
#define AUDIO_BUF_ADDR (SP_START_ADDR + STACK_SIZE)

#define PC_START_ADDR_DEFAULT 0x200
#define CPU_FREQ_DEFAULT 1000
#define REFRESH_FREQ_DEFAULT 60
#define TIMER_FREQ_DEFAULT 60
#define PITCH_DEFAULT 64

// The states each key of the keypad can be in.
typedef enum
{
    KEY_UP,
    KEY_DOWN,
    KEY_RELEASED
} CHIP8K;

// The selected bitplane draw/display operations are performed upon.
typedef enum
{
    BPNONE,
    BP1,
    BP2,
    BPBOTH
} CHIP8BP;

typedef struct CHIP8
{
    // Represents random-access memory.
    uint8_t RAM[MAX_RAM];

    // Represents general-purpose 8-bit registers.
    uint8_t V[NUM_REGISTERS];

    // Program counter, stack pointer, and index 16-bit registers.
    uint16_t PC, SP, I;

    // Delay timer and sound timer 8-bit registers.
    uint8_t DT, ST;

    // 8-bit register which controls audio pitch (XO-CHIP Only).
    uint8_t pitch;

    // A monochrome display. A pixel can be either only on or off, no color.
    bool display[DISPLAY_HEIGHT][DISPLAY_WIDTH];

    // A second display for XO-CHIP support.
    bool display2[DISPLAY_HEIGHT][DISPLAY_WIDTH];

    // Represents the bitmask of both displays.
    CHIP8BP bitplane;

    // Represents if a key is down, up, or released.
    CHIP8K keypad[NUM_KEYS];

    // Where the emulator begins reading instructions.
    uint16_t pc_start_addr;

    // These are used for handling CPU and timer speed.
    unsigned long cpu_freq;
    unsigned long timer_freq;
    unsigned long refresh_freq;
    long timer_max_cum;
    long cpu_max_cum;
    long cpu_cum;
    long sound_cum;
    long delay_cum;
    long refresh_max_cum;
    long refresh_cum;
    long total_cycle_time;

#ifndef __LIBRETRO__
#ifdef WIN32
    LARGE_INTEGER win_cycle_time;
    LARGE_INTEGER cur_cycle_start;
    LARGE_INTEGER prev_cycle_start;
    LARGE_INTEGER real_cpu_freq;
#else
    struct timeval cur_cycle_start;
    struct timeval prev_cycle_start;
#endif
#endif

    // The path and filename of currently loaded ROM.
    char ROM_path[MAX_FILEPATH_LEN];

    // The path and filename of associated user flags file (ROM_path.uf).
    char UF_path[MAX_FILEPATH_LEN + 3];

    // The path and filename of associated dump file (ROM_path.dmp).
    char DMP_path[MAX_FILEPATH_LEN + 4];

    // Flags for the various quirky behavior of S-CHIP
    /* Quirks:
        -0: RAM Initialization
        -1: 8xy6/8xyE
        -2: Fx55/Fx65
        -3: Bnnn
        -4: Big Sprite LORES
        -5: 00FE/00FF
        -6: Sprite Wrapping
        -7: Collision Enumeration
        -8: Collision with Bottom of Screen
        -9: Disable undefined VF after logical OR, AND, XOR (VF is set to 0 with this disabled)
    */
    bool quirks[NUM_QUIRKS];

    // Used to signal to main to update the display.
    bool display_updated;

    // Used to signal to main to produce sound.
    bool beep;

    // Used to signal to main to exit the program.
    bool exit;

    // Used to toggle between HI-RES and standard LO-RES modes.
    bool hires;
} CHIP8;

// Set some things to useful default values.
void chip8_init(CHIP8 *chip8, unsigned long cpu_freq, unsigned long timer_freq,
                unsigned long refresh_freq, uint16_t pc_start_addr,
                bool quirks[]);

// Reset the machine.
void chip8_reset(CHIP8 *chip8);

// Soft reset the machine (keep ROM and fonts loaded).
void chip8_soft_reset(CHIP8 *chip8);

// Sets the CPU frequency of the machine.
void chip8_set_cpu_freq(CHIP8 *chip8, unsigned long cpu_freq);

// Sets the timer frequency of the machine.
void chip8_set_timer_freq(CHIP8 *chip8, unsigned long timer_freq);

// Sets the refresh frequency of the machine.
void chip8_set_refresh_freq(CHIP8 *chip8, unsigned long refresh_freq);

// Load hexadecimal font into memory.
void chip8_load_font(CHIP8 *chip8);

// Loads a given ROM into memory.
bool chip8_load_rom(CHIP8 *chip8, char *filename);

// Loads a given ROM into memory from a buffer.
void chip8_load_rom_buffer(CHIP8 *chip8, const void *raw, size_t sz);
  
/* Performs a full cycle of the emulator including executing an instruction and
handling timers. Returns true if instruction was executed
or false if the CPU was sleeping. */
bool chip8_cycle(CHIP8 *chip8);

// Fetches, decodes, and executes the next instruction.
void chip8_execute(CHIP8 *chip8);

// Decrements delay and sound timers at specified frequency.
void chip8_handle_timers(CHIP8 *chip8);

// Updates the total cycle time since last call.
void chip8_update_elapsed_time(CHIP8 *chip8);

// Clears the keypad by setting all keys to up.
void chip8_reset_keypad(CHIP8 *chip8);

// Resets any key that was released to KEY_UP.
void chip8_reset_released_keys(CHIP8 *chip8);

// Clears the display by setting all pixels to off.
void chip8_reset_display(CHIP8 *chip8, CHIP8BP bitplane);

// Clears the RAM.
void chip8_reset_RAM(CHIP8 *chip8);

// Clears all registers.
void chip8_reset_registers(CHIP8 *chip8);

// Reset audio buffer to default sound.
void chip8_reset_audio(CHIP8 *chip8);

// Loads an instruction into memory.
void chip8_load_instr(CHIP8 *chip8, uint16_t instr);

// Performs a draw operation.
void chip8_draw(CHIP8 *chip8, uint8_t x, uint8_t y, uint8_t n, CHIP8BP bitplane);

// Scrolls the display in specified direction by num_pixels.
void chip8_scroll(CHIP8 *chip8, int xdir, int ydir, int num_pixels, CHIP8BP bitplane);

// Waits for a key to be released then stores that key in Vx.
void chip8_wait_key(CHIP8 *chip8, uint8_t x);

// Dump memory to disk.
bool chip8_dump(CHIP8 *chip8);

// Read dump file into memory.
bool chip8_load_dump(CHIP8 *chip8, char *filename);

// Saves/loads user flags to/from disk.
bool chip8_handle_user_flags(CHIP8 *chip8, int num_flags, bool save);

// Skips the next instrtuction.
void chip8_skip_instr(CHIP8 *chip8);

/* Returns a sound frequency based on the current pitch using the formula:
4000 * 2^((pitch - 64) / 48) */
double chip8_get_sound_freq(CHIP8 *chip8);

#endif
