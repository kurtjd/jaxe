#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include "chip8.h"

#define BAD_KEY 0x42
#define AMPLITUDE 28000
#define SAMPLE_RATE 44100

// SDL display globals
int DISPLAY_SCALE = 10;
long ON_COLOR = 0xFFFFFF;
long OFF_COLOR = 0x000000;

// Black magic SDL sound stuff.
void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes)
{
    Sint16 *buffer = (Sint16 *)raw_buffer;
    int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
    int sample_nr = (*(int *)user_data);

    for (int i = 0; i < length; i++, sample_nr++)
    {
        double time = (double)sample_nr / (double)SAMPLE_RATE;
        // render 441 HZ sine wave
        buffer[i] = (Sint16)(AMPLITUDE * sin(2.0f * M_PI * 441.0f * time));
    }
}

// Sets a pixel of the SDL surface to match the emulator.
void set_pixel(SDL_Surface *surface, int x, int y, bool on)
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    pixels[(y * surface->w) + x] = on ? ON_COLOR : OFF_COLOR;
}

// Makes the physical screen match the emulator display.
void draw_display(SDL_Window *window, SDL_Surface *surface, CHIP8 *chip8)
{
    for (int y = 0; y < MAX_HEIGHT; y++)
    {
        for (int x = 0; x < MAX_WIDTH; x++)
        {
            for (int i = 0; i < DISPLAY_SCALE; i++)
            {
                for (int j = 0; j < DISPLAY_SCALE; j++)
                {
                    set_pixel(surface,
                              (x * DISPLAY_SCALE) + j,
                              (y * DISPLAY_SCALE) + i,
                              chip8->display[y][x]);
                }
            }
        }
    }

    SDL_UpdateWindowSurface(window);
}

// Converts an SDL key code to the respective key on the emulator keypad.
unsigned char SDLK_to_hex(SDL_KeyCode key)
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

// Checks for key presses/releases and a quit event.
bool handle_input(SDL_Event *e, CHIP8 *chip8)
{
    while (SDL_PollEvent(e))
    {
        if (e->type == SDL_QUIT)
        {
            return false;
        }

        if (e->type == SDL_KEYUP)
        {
            unsigned char hexkey = SDLK_to_hex(e->key.keysym.sym);
            if (hexkey != BAD_KEY)
            {
                chip8->keypad[hexkey] = KEY_RELEASED;
            }
        }
        else if (e->type == SDL_KEYDOWN)
        {
            unsigned char hexkey = SDLK_to_hex(e->key.keysym.sym);
            if (hexkey != BAD_KEY)
            {
                chip8->keypad[hexkey] = KEY_DOWN;
            }
        }
    }

    return true;
}

int main(int argc, char *argv[])
{
    // Emulator options
    bool LEGACY_MODE = false;
    unsigned int PC_START_ADDR = PC_START_ADDR_DEFAULT;
    int CLOCK_SPEED = CLOCK_SPEED_DEFAULT;

    /* Check command-line arguments. */
    if (argc < 2)
    {
        printf("Usage: ./jace [options] <path-to-ROM>\n");
        return 1;
    }
    else if (argc > 2)
    {
        int opt;
        while ((opt = getopt(argc, argv, "ld:p:c:f:b:")) != -1)
        {
            switch (opt)
            {
            case 'l':
                LEGACY_MODE = true;
                break;
            case 'd':
                DISPLAY_SCALE = atoi(optarg);
                break;
            case 'p':
                PC_START_ADDR = strtol(optarg, NULL, 16);
                break;
            case 'c':
                CLOCK_SPEED = atoi(optarg);
                break;
            case 'f':
                ON_COLOR = strtol(optarg, NULL, 16);
                break;
            case 'b':
                OFF_COLOR = strtol(optarg, NULL, 16);
                break;
            }
        }
    }

    /* Initialize the CHIP8 emulator. */
    CHIP8 chip8;
    chip8_init(&chip8, LEGACY_MODE, CLOCK_SPEED, PC_START_ADDR);
    chip8_load_font(&chip8);

    /* Load ROM into memory. */
    if (!chip8_load_rom(&chip8, argv[argc - 1]))
    {
        fprintf(stderr, "Unable to open ROM file.\n");
        return 1;
    }

    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "Could not initialize SDL.\n");
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("JACE",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          MAX_WIDTH * DISPLAY_SCALE,
                                          MAX_HEIGHT * DISPLAY_SCALE,
                                          SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        fprintf(stderr, "Could not create SDL window.\n");
        return 1;
    }

    SDL_Surface *surface = SDL_GetWindowSurface(window);

    /******* Black Magic SDL Sound *********/
    int sample_nr = 0;

    SDL_AudioSpec want;
    // number of samples per second
    want.freq = SAMPLE_RATE;

    // sample type (here: signed short i.e. 16 bit)
    want.format = AUDIO_S16SYS;

    // only one channel
    want.channels = 1;

    // buffer-size
    want.samples = 2048;

    // function SDL calls periodically to refill the buffer
    want.callback = audio_callback;

    // counter, keeping track of current sample number
    want.userdata = &sample_nr;

    SDL_AudioSpec have;
    if (SDL_OpenAudio(&want, &have) != 0)
    {
        fprintf(stderr, "Could not open audio.\n");
    }
    if (want.format != have.format)
    {
        fprintf(stderr, "Could not get desired audio spec.\n");
    }
    /******************************/

    /* Main Loop */
    SDL_Event e;
    while (handle_input(&e, &chip8))
    {
        chip8_execute(&chip8);
        chip8_handle_timers(&chip8);

        // Prevents wasting time drawing every frame when unnecessary.
        if (chip8.display_updated)
        {
            draw_display(window, surface, &chip8);
        }

        if (chip8.beep)
        {
            SDL_PauseAudio(0);
        }
        else
        {
            SDL_PauseAudio(1);
        }
    }

    /* Free Resources */
    SDL_DestroyWindow(window);
    SDL_CloseAudio();
    SDL_Quit();

    return 0;
}
