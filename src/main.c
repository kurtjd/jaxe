#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
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
#define DISPLAY_COLOR 0xFFFFFF

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

void machine_init(Machine *machine)
{
    for (int addr = 0; addr < MAX_RAM; addr++)
    {
        machine->RAM[addr] = 0x00;
    }

    for (int k = 0; k < MAX_KEYS; k++)
    {
        machine->keypad[k] = false;
    }

    for (int y = 0; y < MAX_HEIGHT; y++)
    {
        for (int x = 0; x < MAX_WIDTH; x++)
        {
            machine->display[y][x] = false;
        }
    }

    machine->DT = 0;
    machine->ST = 0;
    machine->SP = 0;
    machine->PC = PC_START_ADDR;
    machine->I = 0x00;
}

void machine_load_font(Machine *machine)
{
    /* Load hexadecimal font into memory.
    Each hex character is represented by 5 bytes in memory
    with each bit representing a pixel. */

    // 0:
    machine->RAM[FONT_START_ADDR + 0x0] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x1] = 0x90;
    machine->RAM[FONT_START_ADDR + 0x2] = 0x90;
    machine->RAM[FONT_START_ADDR + 0x3] = 0x90;
    machine->RAM[FONT_START_ADDR + 0x4] = 0xF0;
    // 1:
    machine->RAM[FONT_START_ADDR + 0x5] = 0x20;
    machine->RAM[FONT_START_ADDR + 0x6] = 0x60;
    machine->RAM[FONT_START_ADDR + 0x7] = 0x20;
    machine->RAM[FONT_START_ADDR + 0x8] = 0x20;
    machine->RAM[FONT_START_ADDR + 0x9] = 0x70;
    // 2:
    machine->RAM[FONT_START_ADDR + 0xA] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0xB] = 0x10;
    machine->RAM[FONT_START_ADDR + 0xC] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0xD] = 0x80;
    machine->RAM[FONT_START_ADDR + 0xE] = 0xF0;
    // 3:
    machine->RAM[FONT_START_ADDR + 0xF] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x10] = 0x10;
    machine->RAM[FONT_START_ADDR + 0x11] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x12] = 0x10;
    machine->RAM[FONT_START_ADDR + 0x13] = 0xF0;
    // 4:
    machine->RAM[FONT_START_ADDR + 0x14] = 0x90;
    machine->RAM[FONT_START_ADDR + 0x15] = 0x90;
    machine->RAM[FONT_START_ADDR + 0x16] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x17] = 0x10;
    machine->RAM[FONT_START_ADDR + 0x18] = 0x10;
    // 5:
    machine->RAM[FONT_START_ADDR + 0x19] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x1A] = 0x80;
    machine->RAM[FONT_START_ADDR + 0x1B] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x1C] = 0x10;
    machine->RAM[FONT_START_ADDR + 0x1D] = 0xF0;
    // 6:
    machine->RAM[FONT_START_ADDR + 0x1E] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x1F] = 0x80;
    machine->RAM[FONT_START_ADDR + 0x20] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x21] = 0x90;
    machine->RAM[FONT_START_ADDR + 0x22] = 0xF0;
    // 7:
    machine->RAM[FONT_START_ADDR + 0x23] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x24] = 0x10;
    machine->RAM[FONT_START_ADDR + 0x25] = 0x20;
    machine->RAM[FONT_START_ADDR + 0x26] = 0x40;
    machine->RAM[FONT_START_ADDR + 0x27] = 0x40;
    // 8:
    machine->RAM[FONT_START_ADDR + 0x28] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x29] = 0x90;
    machine->RAM[FONT_START_ADDR + 0x2A] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x2B] = 0x90;
    machine->RAM[FONT_START_ADDR + 0x2C] = 0xF0;
    // 9:
    machine->RAM[FONT_START_ADDR + 0x2D] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x2E] = 0x90;
    machine->RAM[FONT_START_ADDR + 0x2F] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x30] = 0x10;
    machine->RAM[FONT_START_ADDR + 0x31] = 0xF0;
    // A:
    machine->RAM[FONT_START_ADDR + 0x32] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x33] = 0x90;
    machine->RAM[FONT_START_ADDR + 0x34] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x35] = 0x90;
    machine->RAM[FONT_START_ADDR + 0x36] = 0x90;
    // B:
    machine->RAM[FONT_START_ADDR + 0x37] = 0xE0;
    machine->RAM[FONT_START_ADDR + 0x38] = 0x90;
    machine->RAM[FONT_START_ADDR + 0x39] = 0xE0;
    machine->RAM[FONT_START_ADDR + 0x3A] = 0x90;
    machine->RAM[FONT_START_ADDR + 0x3B] = 0xE0;
    // C:
    machine->RAM[FONT_START_ADDR + 0x3C] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x3D] = 0x80;
    machine->RAM[FONT_START_ADDR + 0x3E] = 0x80;
    machine->RAM[FONT_START_ADDR + 0x3F] = 0x80;
    machine->RAM[FONT_START_ADDR + 0x40] = 0xF0;
    // D:
    machine->RAM[FONT_START_ADDR + 0x41] = 0xE0;
    machine->RAM[FONT_START_ADDR + 0x42] = 0x90;
    machine->RAM[FONT_START_ADDR + 0x43] = 0x90;
    machine->RAM[FONT_START_ADDR + 0x44] = 0x90;
    machine->RAM[FONT_START_ADDR + 0x45] = 0xE0;
    // E:
    machine->RAM[FONT_START_ADDR + 0x46] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x47] = 0x80;
    machine->RAM[FONT_START_ADDR + 0x48] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x49] = 0x80;
    machine->RAM[FONT_START_ADDR + 0x4A] = 0xF0;
    // F:
    machine->RAM[FONT_START_ADDR + 0x4B] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x4C] = 0x80;
    machine->RAM[FONT_START_ADDR + 0x4D] = 0xF0;
    machine->RAM[FONT_START_ADDR + 0x4E] = 0x80;
    machine->RAM[FONT_START_ADDR + 0x4F] = 0x80;
}

bool machine_load_rom(Machine *machine, char *filename)
{
    /* Loads a given ROM into memory starting at address 0x200.
    0x200 is where user data is stored, eeverything before that is system. */

    FILE *rom = fopen(filename, "rb");
    if (rom)
    {
        fread(machine->RAM + PC_START_ADDR, MAX_RAM - PC_START_ADDR, 1, rom);
        fclose(rom);

        return true;
    }
    else
    {
        return false;
    }
}

void machine_execute(Machine *machine)
{
    // The first and second byte of instruction respectively.
    unsigned char b1 = machine->RAM[machine->PC], b2 = machine->RAM[machine->PC + 1];

    // The code (first 4 bits) of instruction.
    unsigned char C = b1 >> 4;

    // The last 4 bits of first byte of instruction.
    unsigned char X = b1 & 0xF;

    // The first 4 bits of second byte of instruction.
    unsigned char Y = b2 >> 4;

    // The last 12 bits of instruction (usually an address).
    unsigned int NNN = ((b1 & 0xF) << 8) | b2;

    // The last 8 bits of instruction.
    unsigned char NN = b2;

    // The last 4 bits of instruction.
    unsigned char N = b2 & 0xF;

    // CLS (00E0):
    if (b1 == 0x00 && b2 == 0xE0)
    {
        for (int row = 0; row < MAX_HEIGHT; row++)
        {
            for (int col = 0; col < MAX_WIDTH; col++)
            {
                machine->display[row][col] = false;
            }
        }
    }

    // RET (00EE):
    else if (b1 == 0x00 && b2 == 0xEE)
    {
        machine->PC = machine->stack[machine->SP];
        machine->SP--;
    }

    switch (C)
    {
    // JMP NNN (1NNN):
    case 0x01:
        machine->PC = NNN;
        return;
        break;

    // CALL NNN (2NNN):
    case 0x02:
        machine->SP++;
        machine->stack[machine->SP] = machine->PC;
        machine->PC = NNN;
        break;

    // SE VX, NN (3XNN):
    case 0x03:
        if (machine->V[X] == NN)
        {
            machine->PC += 2;
        }

        break;

    // SNE VX, NN (4XNN):
    case 0x04:
        if (machine->V[X] != NN)
        {
            machine->PC += 2;
        }

        break;

    // SE VX, VY (5XY0):
    case 0x05:
        if (machine->V[X] == machine->V[Y])
        {
            machine->PC += 2;
        }

        break;

    // LD VX, NN (6XNN):
    case 0x06:
        machine->V[X] = NN;
        break;

    // ADD VX, NN (7XNN):
    case 0x07:
        machine->V[X] += NN;
        break;

    // Bitwise family
    case 0x08:
        switch (N)
        {
        // LD VX, VY (8XY0):
        case 0x00:
            machine->V[X] = machine->V[Y];
            break;

        // OR VX, VY (8XY1):
        case 0x01:
            machine->V[X] |= machine->V[Y];
            break;

        // AND VX, VY (8XY2):
        case 0x02:
            machine->V[X] &= machine->V[Y];
            break;

        // XOR VX, VY (8XY3):
        case 0x03:
            machine->V[X] ^= machine->V[Y];
            break;

        // ADD VX, VY (8XY4):
        case 0x04:
            machine->V[0x0F] = ((int)(machine->V[X]) + (int)(machine->V[Y]) > 0xFF) ? 1 : 0;
            machine->V[X] += machine->V[Y];
            break;

        // SUB VX, VY (8XY5):
        case 0x05:
            machine->V[0x0F] = (machine->V[X] > machine->V[Y]) ? 1 : 0;
            machine->V[X] -= machine->V[Y];
            break;

        // SHR VX {, VY} (8XY6):
        case 0x06:
            machine->V[X] = machine->V[Y];
            machine->V[0x0F] = machine->V[X] & 0x01;
            machine->V[X] >>= 1;
            break;

        // SUBN VX, VY (8XY7):
        case 0x07:
            machine->V[0x0F] = (machine->V[Y] > machine->V[X]) ? 1 : 0;
            machine->V[X] = machine->V[Y] - machine->V[X];
            break;

        // SHL VX {, VY} (8XYE):
        case 0x0E:
            machine->V[X] = machine->V[Y];
            machine->V[0x0F] = machine->V[X] & 0x80;
            machine->V[X] <<= 1;
            break;

        default:
            break;
        }

        break;

    // SNE VX, VY (9XY0):
    case 0x09:
        if (machine->V[X] != machine->V[Y])
        {
            machine->PC += 2;
        }

        break;

    // LD I, NNN (ANNN):
    case 0x0A:
        machine->I = NNN;
        break;

    // JMP V0, NNN (BNNN):
    case 0x0B:
        machine->PC = machine->V[0] + NNN;
        break;

    // RND VX, NN (CXNN):
    case 0x0C:
        machine->V[X] = (rand() % 256) & NN;
        break;

    // DRW VX, VY, N (DXYN):
    case 0x0D:
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                int disp_x = machine->V[X] + j;
                int disp_y = machine->V[Y] + i;

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
                bool pixel_on = machine->display[disp_y][disp_x];
                bool bit = (machine->RAM[machine->I + i] >> (7 - j)) & 0x01;

                /* XOR the sprite onto display. 
                If a pixel is erased, set the VF register to 1. */
                machine->display[disp_y][disp_x] = (pixel_on != bit);
                machine->V[0x0F] = (pixel_on && bit) ? 1 : 0;
            }
        }

        break;

    // Key skip family
    case 0x0E:
        switch (b2)
        {
        // SKP VX (EX9E):
        case 0x9E:
            if (machine->keypad[machine->V[X]])
            {
                machine->PC += 2;
            }

            break;

        // SKNP VX (EXA1):
        case 0xA1:
            if (!machine->keypad[machine->V[X]])
            {
                machine->PC += 2;
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
        // LD VX, DT (FX07):
        case 0x07:
            machine->V[X] = machine->DT;
            break;

        // LD VX, K (FX0A):
        case 0x0A:
        {
            bool key_pressed = false;

            for (int i = 0; i < MAX_KEYS; i++)
            {
                if (machine->keypad[i])
                {
                    machine->V[X] = i;
                    key_pressed = true;
                    break;
                }
            }

            if (!key_pressed)
            {
                return;
            }

            break;
        }

        // LD DT, VX (FX15):
        case 0x15:
            machine->DT = machine->V[X];
            break;

        // LD ST, VX (FX18):
        case 0x18:
            machine->ST = machine->V[X];
            break;

        // ADD I, VX (FX1E):
        case 0x1E:
            machine->I += machine->V[X];
            break;

        // LD F, VX (FX29):
        case 0x29:
            machine->I = machine->V[X] * 0x05;
            break;

        // LD B, VX (FX33):
        case 0x33:
            machine->RAM[machine->I] = (machine->V[X] / 100) % 10;
            machine->RAM[machine->I + 1] = (machine->V[X] / 10) % 10;
            machine->RAM[machine->I + 2] = machine->V[X] % 10;
            break;

        // LD [I], VX (FX55):
        case 0x55:
            for (int r = 0; r <= X; r++)
            {
                machine->RAM[machine->I + r] = machine->V[r];
            }

            break;

        // LD VX, [I] (FX65):
        case 0x65:
            for (int r = 0; r <= X; r++)
            {
                machine->V[r] = machine->RAM[machine->I + r];
            }

            break;

        default:
            break;
        }

        break;

    default:
        break;
    }

    machine->PC += 2;
}

void machine_beep()
{
    return;
}

void machine_handle_timers(Machine *machine)
{
    // Decrement timers at a frequency of 60Hz and play sound if needed.
    if (clock() % (CLOCKS_PER_SEC / 60))
    {
        if (machine->DT > 0)
        {
            machine->DT--;
        }
        if (machine->ST > 0)
        {
            machine->ST--;
            machine_beep();
        }
    }
}

void set_pixel(SDL_Surface *surface, int x, int y, bool on)
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    pixels[(y * surface->w) + x] = on ? DISPLAY_COLOR : 0x000000;
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

int main(int argc, char *argv[])
{
    printf("argc: %d\n", argc);

    srand(time(NULL));

    Machine machine;
    machine_init(&machine);
    machine_load_font(&machine);

    // Load ROM into memory.
    if (!machine_load_rom(&machine, argv[1]))
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
    while (!quit && !(machine.RAM[machine.PC] == NOOP && machine.RAM[machine.PC + 1] == NOOP))
    {
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
                    machine.keypad[SDLK_to_hex(e.key.keysym.sym)] = true;
                }
            }
            else if (e.type == SDL_KEYUP)
            {
                unsigned char hexkey = SDLK_to_hex(e.key.keysym.sym);
                if (hexkey != 42)
                {
                    machine.keypad[SDLK_to_hex(e.key.keysym.sym)] = false;
                }
            }
        }

        for (int y = 0; y < MAX_HEIGHT; y++)
        {
            for (int x = 0; x < MAX_WIDTH; x++)
            {
                for (int i = 0; i < DISPLAY_SCALE; i++)
                {
                    for (int j = 0; j < DISPLAY_SCALE; j++)
                    {
                        set_pixel(surface, (x * DISPLAY_SCALE) + j, (y * DISPLAY_SCALE) + i, machine.display[y][x]);
                    }
                }
            }
        }
        SDL_UpdateWindowSurface(window);

        machine_handle_timers(&machine);
        machine_execute(&machine);

        usleep(1000 / 700);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}