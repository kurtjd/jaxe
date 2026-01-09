#define ALLOW_GETOPTS
#ifdef ALLOW_GETOPTS
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_ttf.h>
#include "chip8.h"

#define BAD_KEY 0x42

#define DBG_STACK_MAX 1000
#define DBG_PANEL_WIDTH 200
#define DBG_PANEL_HEIGHT 320
#define DBG_FONT_FILE "../fonts/dbgfont.ttf"
#define DBG_FONT_SIZE 12

#define DISPLAY_SCALE_DEFAULT 5
#define DISPLAY_SCALE_MAX 20
#define BG_COLOR_DEFAULT 0x000000
#define P1_COLOR_DEFAULT 0xFFFFFF
#define P2_COLOR_DEFAULT 0xAAAAAA
#define OVERLAP_COLOR_DEFAULT 0x555555
#define NUM_COLOR_THEMES (int)(sizeof(color_themes) / sizeof(color_themes[0]))

// Sound
int snd_buf_pntr = 0;

// Emulator
CHIP8 chip8;
char ROM_path[MAX_FILEPATH_LEN];
uint16_t pc_start_addr = PC_START_ADDR_DEFAULT;
unsigned long cpu_freq = CPU_FREQ_DEFAULT;
unsigned long timer_freq = TIMER_FREQ_DEFAULT;
unsigned long refresh_freq = REFRESH_FREQ_DEFAULT;
bool play_sound = false;
bool quirks[NUM_QUIRKS] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
bool load_dmp = false;

// Color/Display
// TODO: Add more themes!
long color_themes[] = {
    BG_COLOR_DEFAULT, P1_COLOR_DEFAULT, P2_COLOR_DEFAULT, OVERLAP_COLOR_DEFAULT,
    0x000000, 0xFFFFFF, 0x000000, 0x000000, // Black and white
    0xFFFFFF, 0x000000, 0x000000, 0x000000, // Inverted black and white
    0x000000, 0xFF0000, 0x000000, 0x000000, // Blood
    0x000000, 0x00FF00, 0x000000, 0x000000, // Hacker
    0x000000, 0x0000FF, 0x000000, 0x000000, // Space
    0xF0907C, 0x962912, 0xF46900, 0xFAA400, // Crazy Orange
    0x100019, 0xFFE1FF, 0xE700EA, 0xE700EA, // Cyberpunk
    0x996600, 0xFFCC00, 0xFF6600, 0x662200, // Octo
    0xF9FFB3, 0x3D8026, 0xABCC47, 0x00131A, // LCD
    0x000000, 0xFF0000, 0xFFFF00, 0xFFFFFF, // Hot Dog
    0xAAAAAA, 0x000000, 0xFFFFFF, 0x666666, // Gray
    0x000000, 0x00FF00, 0xFF0000, 0xFFFF00, // CGA 0
    0x000000, 0xFF00FF, 0x00FFFF, 0xFFFFFF  // CGA 1

};
int color_theme_pntr = 0;
SDL_Window *window = NULL;
SDL_Surface *surface = NULL;
int display_scale = DISPLAY_SCALE_DEFAULT;
long bg_color = BG_COLOR_DEFAULT;
long p1_color = P1_COLOR_DEFAULT;
long p2_color = P2_COLOR_DEFAULT;
long overlap_color = OVERLAP_COLOR_DEFAULT;
TTF_Font *dbg_font = NULL;

// Debugger
/* This stack holds instances of the chip8 emulator after every execution.
It is used to be able to step back the emulator. */
CHIP8 dbg_stack[DBG_STACK_MAX];
int dbg_stack_pntr = 0;
bool debug_mode = false;
bool paused = false;
bool dbg_step = false;
bool dbg_step_back = false;

// Push the emulator state onto the debug stack.
void dbg_stack_push()
{
    dbg_stack_pntr++;

    if (dbg_stack_pntr >= DBG_STACK_MAX)
    {
        dbg_stack_pntr = 0;
    }

    dbg_stack[dbg_stack_pntr] = chip8;
}

// Pop the previous emulator state from debug stack.
void dbg_stack_pop()
{
    dbg_stack_pntr--;

    if (dbg_stack_pntr < 0)
    {
        dbg_stack_pntr = DBG_STACK_MAX - 1;
    }

    chip8 = dbg_stack[dbg_stack_pntr];
    dbg_step = true;
    dbg_step_back = true;
}

// Cycles between color themes.
void cycle_color_theme()
{
    color_theme_pntr += 4;
    if (color_theme_pntr >= NUM_COLOR_THEMES)
    {
        color_theme_pntr = 0;
    }

    bg_color = color_themes[color_theme_pntr];
    p1_color = color_themes[color_theme_pntr + 1];
    p2_color = color_themes[color_theme_pntr + 2];
    overlap_color = color_themes[color_theme_pntr + 3];
}

// Frees all resources and exits.
void clean_exit(int status)
{
    if (debug_mode && dbg_font)
    {
        TTF_CloseFont(dbg_font);
        dbg_font = NULL;
    }

    if (surface)
    {
        SDL_FreeSurface(surface);
        surface = NULL;
    }

    if (window)
    {
        SDL_DestroyWindow(window);
        window = NULL;
    }

    SDL_CloseAudio();
    SDL_Quit();

    exit(status);
}

// Gets the name of the loaded ROM
void get_ROM_name(char *rom_name)
{
#ifdef WIN32
    char delim[] = "\\";
#else
    char delim[] = "/";
#endif

    // Find the string after the last occurrence of / or \ (depending on OS)
    char tmp_path[MAX_FILEPATH_LEN];
    sprintf(tmp_path, "%s", ROM_path);

    char *token = strtok(tmp_path, delim);
    while (token != NULL)
    {
        sprintf(rom_name, "%s", token);
        token = strtok(NULL, delim);
    }

    // Find the string before the first occurrence of . and that's the name!
    strtok(rom_name, ".");
}

// Creates a square sound wave based off the MSB of the emulator's sound buffer.
void squarewave_callback(void *snddata, Uint8 *buffer, int bytes)
{
    (void)snddata;

    for (int i = 0; i < bytes; i++)
    {
        // Get the byte of the emulator's buffer that the next sample is in.
        buffer[i] = chip8.RAM[AUDIO_BUF_ADDR + (snd_buf_pntr / 8)];

        // Get the actual sample bit.
        buffer[i] <<= (snd_buf_pntr % 8);
        buffer[i] &= 0x80;

        // Finally set the buffer to max volume if sample is 1.
        buffer[i] *= 0xFF;

        /* Keep track of where we are in the emulator's sound buffer and
        wrap back around if necessary. */
        snd_buf_pntr++;
        if (snd_buf_pntr >= (AUDIO_BUF_SIZE * 8))
        {
            snd_buf_pntr = 0;
        }
    }
}

// Start playing a sound.
void start_sound()
{
    snd_buf_pntr = 0;

    SDL_AudioSpec want;

    // Returns 4000 * 2^((pitch - 64) / 48)
    want.freq = chip8_get_sound_freq(&chip8);

    // We only need 1 bit so might as well select smallest format
    want.format = AUDIO_U8;

    // No need for stereo so use mono
    want.channels = 1;

    // Not exactly sure why a lower number works better but 2 is lowest possible
    want.samples = 2;

    want.callback = squarewave_callback;

    SDL_AudioSpec have;
    if (SDL_OpenAudio(&want, &have) != 0)
    {
        fprintf(stderr, "Could not open audio: %s.\n", SDL_GetError());
    }

    if (want.format != have.format)
    {
        fprintf(stderr, "Could not get desired audio spec.\n");
    }

    SDL_PauseAudio(0);
}

// Stop playing sound.
void stop_sound()
{
    SDL_PauseAudio(1);
    SDL_CloseAudio();
}

// Initializes SDL.
bool init_SDL()
{
    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

// Checks and processes command-line arguments.
bool handle_args(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: ./jaxe [options] <path-to-ROM>\n");
        return false;
    }
    else if (argc >= 2)
    {
        if (strlen(argv[argc - 1]) >= MAX_FILEPATH_LEN)
        {
            fprintf(stderr, "ROM path must be less than %d characters.\n",
                    MAX_FILEPATH_LEN);
            clean_exit(1);
        }

        sprintf(ROM_path, "%s", argv[argc - 1]);

#ifdef ALLOW_GETOPTS
        int opt;
        while ((opt = getopt(argc, argv, "012345678xldms:p:c:t:r:f:b:n:k:")) != -1)
        {
            switch (opt)
            {
            // Toggle specific S-CHIP "quirks"
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                quirks[opt - '0'] = false;
                break;

            // Toggle legacy mode
            case 'l':
                for (size_t i = 0; i < NUM_QUIRKS; i++)
                {
                    quirks[i] = false;
                }

                // Both CHIP-8 and SCHIP apparently clip sprites (but not XO-CHIP)
                quirks[6] = true;

                break;
            
            // Toggle XO-CHIP mode
            case 'x':
                /* XO-CHIP is similar to legacy mode except the last quirk
                   (VF Reset) should not be enabled and sprite wrapping should
                   be enabled. */
                for (size_t i = 0; i < NUM_QUIRKS - 1; i++)
                {
                    quirks[i] = false;
                }
                break;

            // Toggle debug mode
            case 'd':
                debug_mode = true;
                paused = true;
                break;

            // Specify to emulator to load dump file as opposed to ROM
            case 'm':
                load_dmp = true;
                break;

            // Set display scale
            case 's':
                display_scale = atoi(optarg);
                if (display_scale > DISPLAY_SCALE_MAX)
                {
                    display_scale = DISPLAY_SCALE_MAX;
                }

                break;

            // Set the address emulator begins executing at
            case 'p':
                pc_start_addr = strtol(optarg, NULL, 16);
                break;

            // Set CPU frequency
            case 'c':
                cpu_freq = atoi(optarg);
                break;

            // Set timer frequency
            case 't':
                timer_freq = atoi(optarg);
                break;

            // Set screen refresh frequency
            case 'r':
                refresh_freq = atoi(optarg);
                break;

            // Set background color
            case 'b':
                color_themes[0] = strtol(optarg, NULL, 16);
                bg_color = color_themes[0];
                break;

            // Set plane 1 color
            case 'f':
                color_themes[1] = strtol(optarg, NULL, 16);
                p1_color = color_themes[1];
                break;

            // Set plane 2 color
            case 'k':
                color_themes[2] = strtol(optarg, NULL, 16);
                p2_color = color_themes[2];
                break;

            // Set overlap color
            case 'n':
                color_themes[3] = strtol(optarg, NULL, 16);
                overlap_color = color_themes[3];
                break;
            }
        }
#endif
    }

    return true;
}

// Set up the emulator to begin running.
bool init_emulator()
{
    /* If a dump file is given, skip initialization since dump contains
    all necessary data. */
    if (!load_dmp)
    {
        chip8_init(&chip8, cpu_freq, timer_freq, refresh_freq, pc_start_addr,
                   quirks);
        chip8_load_font(&chip8);

        /* Load ROM into memory. */
        if (!chip8_load_rom(&chip8, ROM_path))
        {
            return false;
        }
    }
    else if (!chip8_load_dump(&chip8, ROM_path))
    {
        return false;
    }

    // Initialize the dbg stack with instances of the initial emulator state.
    for (int i = 0; i < DBG_STACK_MAX; i++)
    {
        dbg_stack[i] = chip8;
    }

    return true;
}

// Create the SDL window.
SDL_Window *create_window()
{
    int window_width = DISPLAY_WIDTH * display_scale;
    int window_height = DISPLAY_HEIGHT * display_scale;

    if (debug_mode)
    {
        // Change window size depending on if debug_mode is active or not.
        window_width += DBG_PANEL_WIDTH;
        if ((display_scale * DISPLAY_HEIGHT) < DBG_PANEL_HEIGHT)
        {
            window_height = DBG_PANEL_HEIGHT;
        }

        // Initialize fonts since debug mode relies on them.
        if (TTF_Init() == -1)
        {
            fprintf(stderr, "Could not initialize SDL_ttf: %s\n",
                    SDL_GetError());
            return NULL;
        }

        dbg_font = TTF_OpenFont(DBG_FONT_FILE, DBG_FONT_SIZE);
        if (!dbg_font)
        {
            fprintf(stderr, "Could not load font: %s\n", SDL_GetError());
        }
    }

    char rom_name[MAX_FILEPATH_LEN];
    char title[MAX_FILEPATH_LEN + 10];
    get_ROM_name(rom_name);
    sprintf(title, "JAXE - %s", rom_name);

    SDL_Window *new_window = SDL_CreateWindow(title,
                                              SDL_WINDOWPOS_CENTERED,
                                              SDL_WINDOWPOS_CENTERED,
                                              window_width,
                                              window_height,
                                              SDL_WINDOW_SHOWN);
    if (!new_window)
    {
        fprintf(stderr, "Could not create SDL window: %s\n", SDL_GetError());
        return NULL;
    }

    return new_window;
}

// Sets a pixel of the SDL surface to a certain color.
void set_pixel(int x, int y, long color)
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    pixels[(y * surface->w) + x] = color;
}

// Makes the physical screen match the emulator display.
void draw_display()
{
    for (int y = 0; y < DISPLAY_HEIGHT; y++)
    {
        for (int x = 0; x < DISPLAY_WIDTH; x++)
        {
            for (int i = 0; i < display_scale; i++)
            {
                for (int j = 0; j < display_scale; j++)
                {
                    int sdl_x = (x * display_scale) + j;
                    int sdl_y = (y * display_scale) + i;
                    long color;

                    if (!chip8.display[y][x] && !chip8.display2[y][x])
                    {
                        color = bg_color;
                    }
                    else if (chip8.display[y][x] && !chip8.display2[y][x])
                    {
                        color = p1_color;
                    }
                    else if (!chip8.display[y][x] && chip8.display2[y][x])
                    {
                        color = p2_color;
                    }
                    else if (chip8.display[y][x] && chip8.display2[y][x])
                    {
                        color = overlap_color;
                    }

                    set_pixel(sdl_x, sdl_y, color);
                }
            }
        }
    }

    SDL_UpdateWindowSurface(window);
}

/* Display the debug panel.
This function is nasty and slow as hell, I am not proud of it. */
void draw_debug()
{
    // Create a gray rectangle surface as the side panel for debug.
    SDL_Surface *dbg_panel = SDL_CreateRGBSurface(0,
                                                  DBG_PANEL_WIDTH,
                                                  DBG_PANEL_HEIGHT,
                                                  32, 0, 0, 0, 0);
    SDL_FillRect(dbg_panel, NULL, SDL_MapRGB(dbg_panel->format, 200, 200, 200));

    SDL_Rect dest_rect;
    dest_rect.x = (DISPLAY_WIDTH * display_scale) + 1;
    dest_rect.y = 0;
    dest_rect.w = DBG_PANEL_WIDTH - 1;
    dest_rect.h = DBG_PANEL_HEIGHT;

    // Now create text with useful information.
    SDL_Surface *txt = NULL;
    SDL_Color font_color;
    font_color.a = 0;
    SDL_Rect font_dest_rect;
    font_dest_rect.w = 0;
    font_dest_rect.h = 0;
    char dbg_str[32];

    // [DEBUG]
    font_color.r = 0;
    font_color.g = 0;
    font_color.b = 0;
    font_dest_rect.x = 57;
    font_dest_rect.y = 5;
    txt = TTF_RenderText_Solid(dbg_font, "[DEBUG]", font_color);
    SDL_BlitSurface(txt, NULL, dbg_panel, &font_dest_rect);
    SDL_FreeSurface(txt);

    // OPCODE
    font_color.r = 255;
    font_color.g = 0;
    font_color.b = 0;
    font_dest_rect.x = 41;
    font_dest_rect.y = 30;
    sprintf(dbg_str, "Next: %02X%02X", chip8.RAM[chip8.PC],
            chip8.RAM[chip8.PC + 1]);
    txt = TTF_RenderText_Solid(dbg_font, dbg_str, font_color);
    SDL_BlitSurface(txt, NULL, dbg_panel, &font_dest_rect);
    SDL_FreeSurface(txt);

    // Address Holders
    font_color.r = 0;
    font_color.g = 100;
    font_color.b = 0;

    // PC
    font_dest_rect.x = 57;
    font_dest_rect.y = 56;
    sprintf(dbg_str, "PC: %03X", chip8.PC);
    txt = TTF_RenderText_Solid(dbg_font, dbg_str, font_color);
    SDL_BlitSurface(txt, NULL, dbg_panel, &font_dest_rect);
    SDL_FreeSurface(txt);

    // SP, I
    font_dest_rect.x = 13;
    font_dest_rect.y = 76;
    sprintf(dbg_str, "SP: %03X I: %03X", chip8.SP, chip8.I);
    txt = TTF_RenderText_Solid(dbg_font, dbg_str, font_color);
    SDL_BlitSurface(txt, NULL, dbg_panel, &font_dest_rect);
    SDL_FreeSurface(txt);

    // Timers
    font_color.r = 128;
    font_color.g = 0;
    font_color.b = 128;
    font_dest_rect.x = 17;
    font_dest_rect.y = 97;
    sprintf(dbg_str, "DT: %02X ST: %02X", chip8.DT, chip8.ST);
    txt = TTF_RenderText_Solid(dbg_font, dbg_str, font_color);
    SDL_BlitSurface(txt, NULL, dbg_panel, &font_dest_rect);
    SDL_FreeSurface(txt);

    // Registers
    font_color.r = 0;
    font_color.g = 0;
    font_color.b = 255;
    font_dest_rect.x = 18;
    font_dest_rect.y = 120;

    for (int i = 0; i < (NUM_REGISTERS / 2); i++)
    {
        sprintf(dbg_str, "V%X: %02X V%X: %02X", i, chip8.V[i], i + 8,
                chip8.V[i + 8]);
        txt = TTF_RenderText_Solid(dbg_font, dbg_str, font_color);

        SDL_BlitSurface(txt, NULL, dbg_panel, &font_dest_rect);
        SDL_FreeSurface(txt);

        font_dest_rect.y += 15;
    }

    // Instructions
    font_color.r = 0;
    font_color.g = 0;
    font_color.b = 0;
    font_dest_rect.x = 4;

    // UP
    font_dest_rect.y = 120 + (15 * 8) + 20;
    txt = TTF_RenderText_Solid(dbg_font, "UP/DWN: Fwd/Bk", font_color);
    SDL_BlitSurface(txt, NULL, dbg_panel, &font_dest_rect);
    SDL_FreeSurface(txt);

    // SPACE
    font_dest_rect.y = 120 + (15 * 8) + 36;
    txt = TTF_RenderText_Solid(dbg_font, "SPACE: Strt/Stop", font_color);
    SDL_BlitSurface(txt, NULL, dbg_panel, &font_dest_rect);
    SDL_FreeSurface(txt);

    // ENTER
    font_dest_rect.y = 120 + (15 * 8) + 52;
    txt = TTF_RenderText_Solid(dbg_font, "ENTER: Dump Mem", font_color);
    SDL_BlitSurface(txt, NULL, dbg_panel, &font_dest_rect);
    SDL_FreeSurface(txt);

    // Finally blit debug panel onto window.
    SDL_BlitSurface(dbg_panel, NULL, surface, &dest_rect);
    SDL_FreeSurface(dbg_panel);
    SDL_UpdateWindowSurface(window);
}

// Converts an SDL key code to the respective key on the emulator keypad.
unsigned char SDLK_to_hex(SDL_Keycode key)
{
    switch (key)
    {
    case SDLK_1:
        return 0x01;
        break;
    case SDLK_2:
        return 0x02;
        break;
    case SDLK_3:
        return 0x03;
        break;
    case SDLK_4:
        return 0x0C;
        break;
    case SDLK_q:
        return 0x04;
        break;
    case SDLK_w:
        return 0x05;
        break;
    case SDLK_e:
        return 0x06;
        break;
    case SDLK_r:
        return 0x0D;
        break;
    case SDLK_a:
        return 0x07;
        break;
    case SDLK_s:
        return 0x08;
        break;
    case SDLK_d:
        return 0x09;
        break;
    case SDLK_f:
        return 0x0E;
        break;
    case SDLK_z:
        return 0x0A;
        break;
    case SDLK_x:
        return 0x00;
        break;
    case SDLK_c:
        return 0x0B;
        break;
    case SDLK_v:
        return 0x0F;
        break;
    default:
        return BAD_KEY;
        break;
    }
}

// Handles sound.
void handle_sound()
{
    if (!play_sound && chip8.beep)
    {
        play_sound = true;
        start_sound();
    }
    else if (play_sound && !chip8.beep)
    {
        play_sound = false;
        stop_sound();
    }
}

// Handles drawing the display.
void handle_display()
{
    if (chip8.display_updated)
    {
        draw_display();
    }

    if (debug_mode)
    {
        draw_debug();
    }
}

// Checks for key presses/releases and a quit event.
bool handle_input(SDL_Event *e)
{
    while (SDL_PollEvent(e))
    {
        SDL_Keycode keyc;
        unsigned char hexkey;

        switch (e->type)
        {
        case SDL_QUIT:
            return false;
            break;

        case SDL_KEYUP:
            hexkey = SDLK_to_hex(e->key.keysym.sym);
            keyc = e->key.keysym.sym;

            // Send key press to emulator
            if (hexkey != BAD_KEY)
            {
                chip8.keypad[hexkey] = KEY_RELEASED;
                return true;
            }

            switch (keyc)
            {
            // Start or stop emulator
            case SDLK_SPACE:
                paused = !paused;
                break;

            // Step forward in program
            case SDLK_UP:
                dbg_step = true && debug_mode;
                break;

            // Step backwards in program
            case SDLK_DOWN:
                if (debug_mode)
                {
                    dbg_stack_pop();
                }

                break;

            // Increase CPU frequency
            case SDLK_RIGHT:
                chip8_set_cpu_freq(&chip8, chip8.cpu_freq + 100);
                break;

            // Decrease CPU frequency
            case SDLK_LEFT:
                chip8_set_cpu_freq(&chip8, chip8.cpu_freq - 100);
                break;

            // Dump memory to disk
            case SDLK_RETURN:
                chip8_dump(&chip8);
                break;

            // Change color theme
            case SDLK_BACKSPACE:
                cycle_color_theme();
                break;

            // Reset emulator
            case SDLK_ESCAPE:
                chip8_soft_reset(&chip8);
                break;
            }

            break;

        case SDL_KEYDOWN:
            hexkey = SDLK_to_hex(e->key.keysym.sym);
            if (hexkey != BAD_KEY)
            {
                chip8.keypad[hexkey] = KEY_DOWN;
            }

            break;
        }
    }

    return true;
}

int main(int argc, char **argv)
{
    if (!handle_args(argc, argv))
    {
        clean_exit(1);
    }

    if (!init_emulator(&chip8))
    {
        clean_exit(1);
    }

    if (!init_SDL())
    {
        clean_exit(1);
    }

    window = create_window();
    if (!window)
    {
        clean_exit(1);
    }

    surface = SDL_GetWindowSurface(window);
    if (!surface)
    {
        fprintf(stderr, "Could not create SDL surface: %s\n", SDL_GetError());
        clean_exit(1);
    }

    SDL_Event e;
    while (!chip8.exit && handle_input(&e))
    {
        if ((!paused || dbg_step) && !dbg_step_back)
        {
            /* Push the state of the emulator into debug stack if the CPU
            actually executed an instruction and wasn't sleeping. */
            if (chip8_cycle(&chip8))
            {
                dbg_stack_push();
            }
        }

        handle_sound();
        handle_display();

        dbg_step = false;
        dbg_step_back = false;
    }

    clean_exit(0);

    return 0;
}
