#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "chip8.h"

#define DISPLAY_SCALE 10
#define ON_COLOR 0xFFFFFF
#define OFF_COLOR 0x000000
#define BAD_KEY 0x42

// Called while the emulator is producing sound.
void beep(Mix_Chunk *snd)
{
    Mix_PlayChannel(-1, snd, 0);
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
    /* Check command-line arguments. */
    if (argc < 2)
    {
        printf("Usage: ./jace <path-to-ROM>\n");
        return 1;
    }

    /* Initialize the CHIP8 emulator. */
    CHIP8 chip8;
    chip8_init(&chip8);
    chip8_load_font(&chip8);

    /* Load ROM into memory. */
    if (!chip8_load_rom(&chip8, argv[1]))
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

    /***** TEMPORARY AUDIO CODE *****/
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        fprintf(stderr,
                "SDL_mixer could not initialize! SDL_mixer Error: %s\n",
                Mix_GetError());
    }
    Mix_Chunk *beep_snd = Mix_LoadWAV("../beep.wav");
    if (beep_snd == NULL)
    {
        fprintf(stderr, "Could not load beep.\n");
    }
    /***** END TEMPORARY AUDIO CODE *****/

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
            beep(beep_snd);
        }
    }

    /* Free Resources */
    SDL_DestroyWindow(window);
    Mix_FreeChunk(beep_snd);
    SDL_Quit();

    return 0;
}
