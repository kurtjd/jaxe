#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>

#define MAX_WIDTH 64
#define MAX_HEIGHT 32
#define MAX_KEYS 16
#define MAX_RAM 4096
#define MAX_REGISTERS 16
#define MAX_STACK_LEVELS 16

#define FONT_START_ADDR 0x0
#define PC_START_ADDR 0x200
#define NOOP 0x00
#define DISPLAY_SCALE 10

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
        return 0xC;
        break;
    case SDLK_v:
        return 0xF;
        break;
    default:
        return 42;
        break;
    }
}

void beep()
{
    return;
}

bool load_rom(char *filename, unsigned char RAM[])
{
    /* Loads a given ROM into memory starting at address 0x200.
    0x200 is where user data is stored, eeverything before that is system. */

    FILE *rom = fopen(filename, "rb");
    if (rom)
    {
        fread(RAM + PC_START_ADDR, MAX_RAM - PC_START_ADDR, 1, rom);
        fclose(rom);

        return true;
    }
    else
    {
        return false;
    }
}

void load_font(unsigned char RAM[])
{
    /* Load hexadecimal font into memory.
    Each hex character is represented by 5 bytes in memory
    with each bit representing a pixel. */

    // 0:
    RAM[FONT_START_ADDR + 0x0] = 0xF0;
    RAM[FONT_START_ADDR + 0x1] = 0x90;
    RAM[FONT_START_ADDR + 0x2] = 0x90;
    RAM[FONT_START_ADDR + 0x3] = 0x90;
    RAM[FONT_START_ADDR + 0x4] = 0xF0;
    // 1:
    RAM[FONT_START_ADDR + 0x5] = 0x20;
    RAM[FONT_START_ADDR + 0x6] = 0x60;
    RAM[FONT_START_ADDR + 0x7] = 0x20;
    RAM[FONT_START_ADDR + 0x8] = 0x20;
    RAM[FONT_START_ADDR + 0x9] = 0x70;
    // 2:
    RAM[FONT_START_ADDR + 0xA] = 0xF0;
    RAM[FONT_START_ADDR + 0xB] = 0x10;
    RAM[FONT_START_ADDR + 0xC] = 0xF0;
    RAM[FONT_START_ADDR + 0xD] = 0x80;
    RAM[FONT_START_ADDR + 0xE] = 0xF0;
    // 3:
    RAM[FONT_START_ADDR + 0xF] = 0xF0;
    RAM[FONT_START_ADDR + 0x10] = 0x10;
    RAM[FONT_START_ADDR + 0x11] = 0xF0;
    RAM[FONT_START_ADDR + 0x12] = 0x10;
    RAM[FONT_START_ADDR + 0x13] = 0xF0;
    // 4:
    RAM[FONT_START_ADDR + 0x14] = 0x90;
    RAM[FONT_START_ADDR + 0x15] = 0x90;
    RAM[FONT_START_ADDR + 0x16] = 0xF0;
    RAM[FONT_START_ADDR + 0x17] = 0x10;
    RAM[FONT_START_ADDR + 0x18] = 0x10;
    // 5:
    RAM[FONT_START_ADDR + 0x19] = 0xF0;
    RAM[FONT_START_ADDR + 0x1A] = 0x80;
    RAM[FONT_START_ADDR + 0x1B] = 0xF0;
    RAM[FONT_START_ADDR + 0x1C] = 0x10;
    RAM[FONT_START_ADDR + 0x1D] = 0xF0;
    // 6:
    RAM[FONT_START_ADDR + 0x1E] = 0xF0;
    RAM[FONT_START_ADDR + 0x1F] = 0x80;
    RAM[FONT_START_ADDR + 0x20] = 0xF0;
    RAM[FONT_START_ADDR + 0x21] = 0x90;
    RAM[FONT_START_ADDR + 0x22] = 0xF0;
    // 7:
    RAM[FONT_START_ADDR + 0x23] = 0xF0;
    RAM[FONT_START_ADDR + 0x24] = 0x10;
    RAM[FONT_START_ADDR + 0x25] = 0x20;
    RAM[FONT_START_ADDR + 0x26] = 0x40;
    RAM[FONT_START_ADDR + 0x27] = 0x40;
    // 8:
    RAM[FONT_START_ADDR + 0x28] = 0xF0;
    RAM[FONT_START_ADDR + 0x29] = 0x90;
    RAM[FONT_START_ADDR + 0x2A] = 0xF0;
    RAM[FONT_START_ADDR + 0x2B] = 0x90;
    RAM[FONT_START_ADDR + 0x2C] = 0xF0;
    // 9:
    RAM[FONT_START_ADDR + 0x2D] = 0xF0;
    RAM[FONT_START_ADDR + 0x2E] = 0x90;
    RAM[FONT_START_ADDR + 0x2F] = 0xF0;
    RAM[FONT_START_ADDR + 0x30] = 0x10;
    RAM[FONT_START_ADDR + 0x31] = 0xF0;
    // A:
    RAM[FONT_START_ADDR + 0x32] = 0xF0;
    RAM[FONT_START_ADDR + 0x33] = 0x90;
    RAM[FONT_START_ADDR + 0x34] = 0xF0;
    RAM[FONT_START_ADDR + 0x35] = 0x90;
    RAM[FONT_START_ADDR + 0x36] = 0x90;
    // B:
    RAM[FONT_START_ADDR + 0x37] = 0xE0;
    RAM[FONT_START_ADDR + 0x38] = 0x90;
    RAM[FONT_START_ADDR + 0x39] = 0xE0;
    RAM[FONT_START_ADDR + 0x3A] = 0x90;
    RAM[FONT_START_ADDR + 0x3B] = 0xE0;
    // C:
    RAM[FONT_START_ADDR + 0x3C] = 0xF0;
    RAM[FONT_START_ADDR + 0x3D] = 0x80;
    RAM[FONT_START_ADDR + 0x3E] = 0x80;
    RAM[FONT_START_ADDR + 0x3F] = 0x80;
    RAM[FONT_START_ADDR + 0x40] = 0xF0;
    // D:
    RAM[FONT_START_ADDR + 0x41] = 0xE0;
    RAM[FONT_START_ADDR + 0x42] = 0x90;
    RAM[FONT_START_ADDR + 0x43] = 0x90;
    RAM[FONT_START_ADDR + 0x44] = 0x90;
    RAM[FONT_START_ADDR + 0x45] = 0xE0;
    // E:
    RAM[FONT_START_ADDR + 0x46] = 0xF0;
    RAM[FONT_START_ADDR + 0x47] = 0x80;
    RAM[FONT_START_ADDR + 0x48] = 0xF0;
    RAM[FONT_START_ADDR + 0x49] = 0x80;
    RAM[FONT_START_ADDR + 0x4A] = 0xF0;
    // F:
    RAM[FONT_START_ADDR + 0x4B] = 0xF0;
    RAM[FONT_START_ADDR + 0x4C] = 0x80;
    RAM[FONT_START_ADDR + 0x4D] = 0xF0;
    RAM[FONT_START_ADDR + 0x4E] = 0x80;
    RAM[FONT_START_ADDR + 0x4F] = 0x80;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    // Represents 4096 bytes (4KB) of addressable memory.
    unsigned char RAM[MAX_RAM] = {0x00};

    // Represents 16 general-purpose 8-bit registers (V0-VF).
    unsigned char V[MAX_REGISTERS];

    // Delay timer, sound timer and stack pointer 8-bit registers.
    unsigned char DT = 0, ST = 0, SP = 0;

    // Program counter and index 16-bit registers.
    unsigned int PC = PC_START_ADDR, I = 0x00;

    // The call stack. CHIP-8 allows a max of 16 nested calls.
    unsigned int stack[MAX_STACK_LEVELS];

    /* A monochrome display of 64x32 pixels.
    A pixel can be either only on or off, no color. */
    bool display[MAX_HEIGHT][MAX_WIDTH] = {false};

    // Represents keys 0-F and if they are pressed or not.
    bool keypad[MAX_KEYS] = {false};

    load_font(RAM);

    // Load ROM into memory (for now filename hardcoded).
    if (!load_rom("../roms/my_test.c8", RAM))
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
    while (!quit && !(RAM[PC] == NOOP && RAM[PC + 1] == NOOP))
    {
        // The first and second byte of instruction respectively.
        unsigned char b1 = RAM[PC], b2 = RAM[PC + 1];

        // The code (first 4 bits) of instruction.
        unsigned char C = b1 >> 4;

        // The last 4 bits of first byte of instruction.
        unsigned char X = b1 & 0xF;

        // The first 4 bits of second byte of instruction.
        unsigned char Y = b2 >> 4;

        // The last 12 bits of instruction (usually an address).
        unsigned int NNN = ((b1 & 0xF) << 8) | (int)b2;

        // The last 8 bits of instruction.
        unsigned char NN = b2;

        // The last 4 bits of instruction.
        unsigned char N = b2 & 0xF;

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                unsigned char hexkey = SDLK_to_hex(e.key.keysym.sym);
                if (hexkey != 42)
                {
                    keypad[SDLK_to_hex(e.key.keysym.sym)] = true;
                }
            }
            else if (e.type == SDL_KEYUP)
            {
                unsigned char hexkey = SDLK_to_hex(e.key.keysym.sym);
                if (hexkey != 42)
                {
                    keypad[SDLK_to_hex(e.key.keysym.sym)] = false;
                }
            }
        }

        // Decrement timers at a frequency of 60Hz and play sound if needed.
        if ((clock() % (CLOCKS_PER_SEC / 60)))
        {
            if (DT > 0)
            {
                DT--;
            }
            if (ST > 0)
            {
                ST--;
                beep();
            }
        }

        // CLS:
        if (b1 == 0x00 && b2 == 0xE0)
        {
            for (int row = 0; row < MAX_HEIGHT; row++)
            {
                for (int col = 0; col < MAX_WIDTH; col++)
                {
                    display[row][col] = false;
                }
            }
        }

        // RET:
        else if (b1 == 0x00 && b2 == 0xEE)
        {
            PC = stack[SP];
            SP--;
        }

        switch (C)
        {
        // JMP:
        case 0x01:
            PC = NNN;
            continue;
            break;

        // CALL:
        case 0x02:
            SP++;
            stack[SP] = PC;
            PC = NNN;
            break;

        // SE VX, NN:
        case 0x03:
            if (V[X] == NN)
            {
                PC += 2;
            }

            break;

        // SNE VX, NN:
        case 0x04:
            if (V[X] != NN)
            {
                PC += 2;
            }

            break;

        // SE VX, VY:
        case 0x05:
            if (V[X] == V[Y])
            {
                PC += 2;
            }

            break;

        // LD VX, NN:
        case 0x06:
            V[X] = NN;
            break;

        // ADD VX, NN:
        case 0x07:
            V[X] += NN;
            break;

        // Bitwise family
        case 0x08:
            switch (N)
            {
            // LD VX, VY:
            case 0x00:
                V[X] = V[Y];
                break;

            // OR VX, VY
            case 0x01:
                V[X] |= V[Y];
                break;

            // AND VX, VY
            case 0x02:
                V[X] &= V[Y];
                break;

            // XOR VX, VY
            case 0x03:
                V[X] ^= V[Y];
                break;

            // ADD VX, VY
            case 0x04:
                V[0x0F] = ((int)(V[X]) + (int)(V[Y]) > 0xFF) ? 1 : 0;
                V[X] += V[Y];
                break;

            // SUB VX, VY
            case 0x05:
                V[0x0F] = (V[X] > V[Y]) ? 1 : 0;
                V[X] -= V[Y];
                break;

            // SHR VX {, VY}
            case 0x06:
                V[0xF] = V[X] & 0x01;
                V[X] = V[X] >> 1;
                break;

            // SUBN VX, VY
            case 0x07:
                V[0x0F] = (V[Y] > V[X]) ? 1 : 0;
                V[X] = V[Y] - V[X];
                break;

            // SHL VX {, VY}
            case 0x0E:
                V[0xF] = V[X] & 0x80;
                V[X] = V[X] << 1;
                break;

            default:
                break;
            }

            break;

        // SNE VX, VY:
        case 0x09:
            if (V[X] != V[Y])
            {
                PC += 2;
            }

            break;

        // LD I, NNN:
        case 0x0A:
            I = NNN;
            break;

        // JMP V0, NNN:
        case 0x0B:
            PC = V[0] + NNN;
            break;

        // RND VX, NN:
        case 0x0C:
            V[X] = (rand() % 256) & NN;
            break;

        // DRW VX, VY, N:
        case 0x0D:
            for (int i = 0; i <= N; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    int disp_x = X + j;
                    int disp_y = Y + i;

                    // Allow out-of-bound sprite to wrap-around.
                    if (disp_x >= MAX_WIDTH)
                    {
                        disp_x -= MAX_WIDTH;
                    }
                    if (disp_y >= MAX_HEIGHT)
                    {
                        disp_y -= MAX_HEIGHT;
                    }

                    // Get the pixel the loop is on and the corresponding bit.
                    bool pixel_on = display[disp_y][disp_x];
                    bool bit = (RAM[I + j] >> (7 - j)) & 0x01;

                    /* XOR the sprite onto display. 
                    If a pixel is erased, set the VF register to 1. */
                    display[disp_y][disp_x] = (pixel_on != bit);
                    V[0x0F] = (pixel_on && bit) ? 1 : 0;
                }
            }

            break;

        // Key skip family
        case 0x0E:
            switch (b2)
            {
            // SKP VX
            case 0x9E:
                if (keypad[V[X]])
                {
                    PC += 2;
                }

                break;

            // SKNP VX
            case 0xA1:
                if (!keypad[V[X]])
                {
                    PC += 2;
                }

                break;

            default:
                break;
            }

            break;

        // F command family
        case 0x0F:
            switch (b2)
            {
            // LD VX, DT
            case 0x07:
                V[X] = input();
                break;

            // LD VX, K
            case 0x0A:
                bool key_pressed = false;

                for (int i = 0; i < MAX_KEYS; i++)
                {
                    if (keypad[i])
                    {
                        V[X] = i;
                        key_pressed = true;
                        break;
                    }
                }

                if (!key_pressed)
                {
                    continue;
                }

                break;

            // LD DT, VX
            case 0x15:
                DT = V[X];
                break;

            // LD ST, VX
            case 0x18:
                ST = V[X];
                break;

            // ADD I, VX
            case 0x1E:
                I += V[X];
                break;

            // LD F, VX
            case 0x29:
                I = V[X] * 0x05;
                break;

            // LD B, VX
            case 0x33:
                RAM[I] = (V[X] / 100) % 10;
                RAM[I + 1] = (V[X] / 10) % 10;
                RAM[I + 2] = V[X] % 10;
                break;

            // LD [I], VX
            case 0x55:
                for (int r = 0; r <= X; r++)
                {
                    RAM[I + r] = V[r];
                }

                break;

            // LD VX, [I]
            case 0x65:
                for (int r = 0; r <= X; r++)
                {
                    V[r] = RAM[I + r];
                }

                break;

            default:
                break;
            }

            break;

        default:
            break;
        }

        PC += 2;
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}