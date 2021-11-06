#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_ttf.h>
#include "chip8.h"

#define BAD_KEY 0x42
#define AMPLITUDE 28000
#define SAMPLE_RATE 44100
#define DBG_PANEL_WIDTH 200
#define DBG_PANEL_HEIGHT 320
#define DBG_FONT_FILE "../fonts/dbgfont.ttf"

// Globals
int DISPLAY_SCALE = 5;
long ON_COLOR = 0xFFFFFF;
long OFF_COLOR = 0x000000;
TTF_Font *DBG_FONT = NULL;

bool DEBUG_MODE = false;
bool dbg_paused = false;
bool dbg_step = false;

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

// Sets a pixel of the SDL surface to a certain color.
void set_pixel(SDL_Surface *surface, int x, int y, long color)
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    pixels[(y * surface->w) + x] = color;
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
                    int sdl_x = (x * DISPLAY_SCALE) + j;
                    int sdl_y = (y * DISPLAY_SCALE) + i;

                    if (chip8->display[y][x])
                    {
                        set_pixel(surface, sdl_x, sdl_y, ON_COLOR);
                    }
                    else
                    {
                        set_pixel(surface, sdl_x, sdl_y, OFF_COLOR);
                    }
                }
            }
        }
    }

    SDL_UpdateWindowSurface(window);
}

/* Display the debug panel.
This function is nasty and slow as hell, I am not proud of it. */
void draw_debug(SDL_Window *window, SDL_Surface *surface, CHIP8 *chip8)
{
    // Create a gray rectangle surface as the side panel for debug.
    SDL_Surface *dbg_panel = SDL_CreateRGBSurface(0,
                                                  DBG_PANEL_WIDTH,
                                                  DBG_PANEL_HEIGHT,
                                                  32, 0, 0, 0, 0);
    SDL_FillRect(dbg_panel, NULL, SDL_MapRGB(dbg_panel->format, 200, 200, 200));

    SDL_Rect dest_rect;
    dest_rect.x = (MAX_WIDTH * DISPLAY_SCALE) + 1;
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
    txt = TTF_RenderText_Solid(DBG_FONT, "[DEBUG]", font_color);
    SDL_BlitSurface(txt, NULL, dbg_panel, &font_dest_rect);
    SDL_FreeSurface(txt);

    // OPCODE
    font_color.r = 255;
    font_color.g = 0;
    font_color.b = 0;
    font_dest_rect.x = 41;
    font_dest_rect.y = 30;
    sprintf(dbg_str, "Next: %02X%02X", chip8->RAM[chip8->PC], chip8->RAM[chip8->PC + 1]);
    txt = TTF_RenderText_Solid(DBG_FONT, dbg_str, font_color);
    SDL_BlitSurface(txt, NULL, dbg_panel, &font_dest_rect);
    SDL_FreeSurface(txt);

    // Address Holders
    font_color.r = 0;
    font_color.g = 100;
    font_color.b = 0;

    // PC
    font_dest_rect.x = 57;
    font_dest_rect.y = 56;
    sprintf(dbg_str, "PC: %03X", chip8->PC);
    txt = TTF_RenderText_Solid(DBG_FONT, dbg_str, font_color);
    SDL_BlitSurface(txt, NULL, dbg_panel, &font_dest_rect);
    SDL_FreeSurface(txt);

    // SP, I
    font_dest_rect.x = 13;
    font_dest_rect.y = 76;
    sprintf(dbg_str, "SP: %03X I: %03X", chip8->SP, chip8->I);
    txt = TTF_RenderText_Solid(DBG_FONT, dbg_str, font_color);
    SDL_BlitSurface(txt, NULL, dbg_panel, &font_dest_rect);
    SDL_FreeSurface(txt);

    // Timers
    font_color.r = 128;
    font_color.g = 0;
    font_color.b = 128;
    font_dest_rect.x = 17;
    font_dest_rect.y = 97;
    sprintf(dbg_str, "DT: %02X ST: %02X", chip8->DT, chip8->ST);
    txt = TTF_RenderText_Solid(DBG_FONT, dbg_str, font_color);
    SDL_BlitSurface(txt, NULL, dbg_panel, &font_dest_rect);
    SDL_FreeSurface(txt);

    // Registers
    font_color.r = 0;
    font_color.g = 0;
    font_color.b = 255;
    font_dest_rect.x = 18;
    font_dest_rect.y = 120;

    for (int i = 0; i < 8; i++)
    {
        sprintf(dbg_str, "V%X: %02X V%X: %02X", i, chip8->V[i], i + 8, chip8->V[i + 8]);
        txt = TTF_RenderText_Solid(DBG_FONT, dbg_str, font_color);

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
    txt = TTF_RenderText_Solid(DBG_FONT, "UP:    Step", font_color);
    SDL_BlitSurface(txt, NULL, dbg_panel, &font_dest_rect);
    SDL_FreeSurface(txt);

    // SPACE
    font_dest_rect.y = 120 + (15 * 8) + 36;
    txt = TTF_RenderText_Solid(DBG_FONT, "SPACE: Strt/Stop", font_color);
    SDL_BlitSurface(txt, NULL, dbg_panel, &font_dest_rect);
    SDL_FreeSurface(txt);

    // ENTER
    font_dest_rect.y = 120 + (15 * 8) + 52;
    txt = TTF_RenderText_Solid(DBG_FONT, "ENTER: Dump Mem", font_color);
    SDL_BlitSurface(txt, NULL, dbg_panel, &font_dest_rect);
    SDL_FreeSurface(txt);

    // Finally blit debug panel onto window.
    SDL_BlitSurface(dbg_panel, NULL, surface, &dest_rect);
    SDL_FreeSurface(dbg_panel);
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
            else if (e->key.keysym.sym == SDLK_SPACE)
            {
                dbg_paused = !dbg_paused;
            }
            else if (e->key.keysym.sym == SDLK_UP)
            {
                dbg_step = true;
            }
            else if (e->key.keysym.sym == SDLK_RETURN)
            {
                if (chip8_dump(chip8))
                {
                    printf("Took a dump in %s\n", chip8->DMP_path);
                }
                else
                {
                    fprintf(stderr, "Unable to take a dump in %s\n", chip8->DMP_path);
                }
            }
            else if (e->key.keysym.sym == SDLK_ESCAPE)
            {
                chip8_soft_reset(chip8);
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

int main(int argc, char **argv)
{
    // Emulator options
    bool LEGACY_MODE = false;
    unsigned int PC_START_ADDR = PC_START_ADDR_DEFAULT;
    int CLOCK_SPEED = CLOCK_SPEED_DEFAULT;
    bool quirks[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    bool load_dmp = false;

    /* Check command-line arguments. */
    if (argc < 2)
    {
        printf("Usage: ./jace [options] <path-to-ROM>\n");
        return 1;
    }
    else if (argc > 2)
    {
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
        */
        int opt;
        while ((opt = getopt(argc, argv, "012345678lodms:p:c:x:y:")) != -1)
        {
            switch (opt)
            {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                quirks[opt - '0'] = false;
                break;
            case 'l':
                LEGACY_MODE = true;
                CLOCK_SPEED = 500;
                break;
            case 'o':
                // Octo mode is essentially HI-RES without any S-CHIP quirks.
                for (size_t i = 0; i < sizeof(quirks); i++)
                {
                    quirks[i] = false;
                }
                break;
            case 'd':
                DEBUG_MODE = true;
                dbg_paused = true;
                break;
            case 'm':
                load_dmp = true;
                break;
            case 's':
                DISPLAY_SCALE = atoi(optarg);
                break;
            case 'p':
                PC_START_ADDR = strtol(optarg, NULL, 16);
                break;
            case 'c':
                CLOCK_SPEED = atoi(optarg);
                break;
            case 'x':
                ON_COLOR = strtol(optarg, NULL, 16);
                break;
            case 'y':
                OFF_COLOR = strtol(optarg, NULL, 16);
                break;
            }
        }
    }

    /* Initialize the CHIP8 emulator. */
    CHIP8 chip8;

    /* If a dump file is given, skip initialization since dump contains
    all necessary data. */
    if (!load_dmp)
    {
        chip8_init(&chip8, LEGACY_MODE, CLOCK_SPEED, PC_START_ADDR, quirks);
        chip8_load_font(&chip8);

        /* Load ROM into memory. */
        if (!chip8_load_rom(&chip8, argv[argc - 1]))
        {
            fprintf(stderr, "Unable to open ROM: %s\n", argv[argc - 1]);
            return 1;
        }
    }
    else if (!chip8_load_dump(&chip8, argv[argc - 1]))
    {
        fprintf(stderr, "Unable to open dump: %s\n", argv[argc - 1]);
        return 1;
    }

    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "Could not initialize SDL.\n");
        return 1;
    }

    int window_width = MAX_WIDTH * DISPLAY_SCALE;
    int window_height = MAX_HEIGHT * DISPLAY_SCALE;
    if (DEBUG_MODE)
    {
        // Change window size depending on if DEBUG_MODE is active or not.
        window_width += DBG_PANEL_WIDTH;
        if ((DISPLAY_SCALE * MAX_HEIGHT) < DBG_PANEL_HEIGHT)
        {
            window_height = DBG_PANEL_HEIGHT;
        }

        // Initialize fonts since debug mode relies on them.
        if (TTF_Init() == -1)
        {
            fprintf(stderr, "Could not initialize SDL_ttf.\n");
            return 1;
        }

        DBG_FONT = TTF_OpenFont(DBG_FONT_FILE, 12);
        if (DBG_FONT == NULL)
        {
            fprintf(stderr, "Could not load font.\n");
        }
    }
    SDL_Window *window = SDL_CreateWindow("JACE",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          window_width,
                                          window_height,
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

    if (DEBUG_MODE)
    {
        draw_debug(window, surface, &chip8);
    }

    /* Main Loop */
    SDL_Event e;
    while (!chip8.exit && handle_input(&e, &chip8))
    {
        if (dbg_paused)
        {
            if (!dbg_step)
            {
                continue;
            }
            else
            {
                dbg_step = false;
            }
        }

        chip8_cycle(&chip8);

        // Prevents wasting time drawing every frame when unnecessary.
        if (chip8.display_updated)
        {
            draw_display(window, surface, &chip8);
        }

        if (DEBUG_MODE)
        {
            draw_debug(window, surface, &chip8);
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
    if (DEBUG_MODE)
    {
        TTF_CloseFont(DBG_FONT);
    }
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_CloseAudio();
    SDL_Quit();

    return 0;
}
