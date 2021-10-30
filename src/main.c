#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "chip8.h"

#define DISPLAY_SCALE 10
#define ON_COLOR 0xFFFFFF
#define OFF_COLOR 0x000000

void set_pixel(SDL_Surface *surface, int x, int y, bool on)
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    pixels[(y * surface->w) + x] = on ? ON_COLOR : OFF_COLOR;
}

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
                    set_pixel(surface, (x * DISPLAY_SCALE) + j, (y * DISPLAY_SCALE) + i, chip8->display[y][x]);
                }
            }
        }
    }
    SDL_UpdateWindowSurface(window);
}

unsigned char SDLK_to_hex(SDL_KeyCode key)
{
    // Maps a key press to the corresponding key on hex pad.
    switch (key)
    {
    case SDLK_1:
        return 0x1;
        break;
    case SDLK_2:
        return 0x2;
        break;
    case SDLK_3:
        return 0x3;
        break;
    case SDLK_4:
        return 0xC;
        break;
    case SDLK_q:
        return 0x4;
        break;
    case SDLK_w:
        return 0x5;
        break;
    case SDLK_e:
        return 0x6;
        break;
    case SDLK_r:
        return 0xD;
        break;
    case SDLK_a:
        return 0x7;
        break;
    case SDLK_s:
        return 0x8;
        break;
    case SDLK_d:
        return 0x9;
        break;
    case SDLK_f:
        return 0xE;
        break;
    case SDLK_z:
        return 0xA;
        break;
    case SDLK_x:
        return 0x0;
        break;
    case SDLK_c:
        return 0xB;
        break;
    case SDLK_v:
        return 0xF;
        break;
    default:
        return 42;
        break;
    }
}

void handle_input(SDL_Event *e, bool *quit, CHIP8 *chip8)
{
    for (int k = 0; k < MAX_KEYS; k++)
    {
        if (chip8->keypad[k] == 2)
        {
            chip8->keypad[k] = 0;
        }
    }

    while (SDL_PollEvent(e))
    {
        if (e->type == SDL_QUIT)
        {
            *quit = true;
        }
        else if (e->type == SDL_KEYUP)
        {
            unsigned char hexkey = SDLK_to_hex(e->key.keysym.sym);
            if (hexkey != 42)
            {
                chip8->keypad[hexkey] = 2;
            }
        }
        else if (e->type == SDL_KEYDOWN)
        {
            unsigned char hexkey = SDLK_to_hex(e->key.keysym.sym);
            if (hexkey != 42)
            {
                chip8->keypad[hexkey] = 1;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    (void)argc; // For now to quit compiler complaining about unused param
    srand(time(NULL));

    CHIP8 chip8;
    chip8_init(&chip8);
    chip8_load_font(&chip8);

    // Load ROM into memory.
    if (!chip8_load_rom(&chip8, argv[1]))
    {
        fprintf(stderr, "Unable to open ROM file.\n");
        return 1;
    }

    SDL_Window *window = NULL;
    SDL_Surface *surface = NULL;
    SDL_Event e;
    bool quit = false;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Could not initialize SDL.\n");
        return 1;
    }

    window = SDL_CreateWindow("JACE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, MAX_WIDTH * DISPLAY_SCALE, MAX_HEIGHT * DISPLAY_SCALE, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        fprintf(stderr, "Could not create SDL window.\n");
        return 1;
    }

    surface = SDL_GetWindowSurface(window);

    // Read and execute instructions from memory until none (0x0000) is found.
    while (!quit && !(chip8.RAM[chip8.PC] == NOOP && chip8.RAM[chip8.PC + 1] == NOOP))
    {
        handle_input(&e, &quit, &chip8);
        chip8_execute(&chip8);
        chip8_handle_timers(&chip8);
        draw_display(window, surface, &chip8);
        //chip8_reset_keypad(&chip8);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}