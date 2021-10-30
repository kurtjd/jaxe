#ifdef WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE < 199309L
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "chip8.h"

void chip8_init(CHIP8 *chip8)
{
    chip8_reset_RAM(chip8);
    chip8_reset_keypad(chip8);
    chip8_reset_display(chip8);

    chip8->PC = PC_START_ADDR;
    chip8->SP = SP_START_ADDR;
    chip8->I = 0x00;
    chip8->DT = 0;
    chip8->ST = 0;

    chip8->display_updated = false;
    chip8->beep = false;
}

void chip8_load_font(CHIP8 *chip8)
{
    /* Load hexadecimal font into memory.
    Each hex character is represented by 5 bytes in memory
    with each bit representing a pixel. */

    // 0:
    chip8->RAM[FONT_START_ADDR + 0x0] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x1] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x2] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x3] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x4] = 0xF0;
    // 1:
    chip8->RAM[FONT_START_ADDR + 0x5] = 0x20;
    chip8->RAM[FONT_START_ADDR + 0x6] = 0x60;
    chip8->RAM[FONT_START_ADDR + 0x7] = 0x20;
    chip8->RAM[FONT_START_ADDR + 0x8] = 0x20;
    chip8->RAM[FONT_START_ADDR + 0x9] = 0x70;
    // 2:
    chip8->RAM[FONT_START_ADDR + 0xA] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0xB] = 0x10;
    chip8->RAM[FONT_START_ADDR + 0xC] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0xD] = 0x80;
    chip8->RAM[FONT_START_ADDR + 0xE] = 0xF0;
    // 3:
    chip8->RAM[FONT_START_ADDR + 0xF] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x10] = 0x10;
    chip8->RAM[FONT_START_ADDR + 0x11] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x12] = 0x10;
    chip8->RAM[FONT_START_ADDR + 0x13] = 0xF0;
    // 4:
    chip8->RAM[FONT_START_ADDR + 0x14] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x15] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x16] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x17] = 0x10;
    chip8->RAM[FONT_START_ADDR + 0x18] = 0x10;
    // 5:
    chip8->RAM[FONT_START_ADDR + 0x19] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x1A] = 0x80;
    chip8->RAM[FONT_START_ADDR + 0x1B] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x1C] = 0x10;
    chip8->RAM[FONT_START_ADDR + 0x1D] = 0xF0;
    // 6:
    chip8->RAM[FONT_START_ADDR + 0x1E] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x1F] = 0x80;
    chip8->RAM[FONT_START_ADDR + 0x20] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x21] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x22] = 0xF0;
    // 7:
    chip8->RAM[FONT_START_ADDR + 0x23] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x24] = 0x10;
    chip8->RAM[FONT_START_ADDR + 0x25] = 0x20;
    chip8->RAM[FONT_START_ADDR + 0x26] = 0x40;
    chip8->RAM[FONT_START_ADDR + 0x27] = 0x40;
    // 8:
    chip8->RAM[FONT_START_ADDR + 0x28] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x29] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x2A] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x2B] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x2C] = 0xF0;
    // 9:
    chip8->RAM[FONT_START_ADDR + 0x2D] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x2E] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x2F] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x30] = 0x10;
    chip8->RAM[FONT_START_ADDR + 0x31] = 0xF0;
    // A:
    chip8->RAM[FONT_START_ADDR + 0x32] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x33] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x34] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x35] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x36] = 0x90;
    // B:
    chip8->RAM[FONT_START_ADDR + 0x37] = 0xE0;
    chip8->RAM[FONT_START_ADDR + 0x38] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x39] = 0xE0;
    chip8->RAM[FONT_START_ADDR + 0x3A] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x3B] = 0xE0;
    // C:
    chip8->RAM[FONT_START_ADDR + 0x3C] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x3D] = 0x80;
    chip8->RAM[FONT_START_ADDR + 0x3E] = 0x80;
    chip8->RAM[FONT_START_ADDR + 0x3F] = 0x80;
    chip8->RAM[FONT_START_ADDR + 0x40] = 0xF0;
    // D:
    chip8->RAM[FONT_START_ADDR + 0x41] = 0xE0;
    chip8->RAM[FONT_START_ADDR + 0x42] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x43] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x44] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x45] = 0xE0;
    // E:
    chip8->RAM[FONT_START_ADDR + 0x46] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x47] = 0x80;
    chip8->RAM[FONT_START_ADDR + 0x48] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x49] = 0x80;
    chip8->RAM[FONT_START_ADDR + 0x4A] = 0xF0;
    // F:
    chip8->RAM[FONT_START_ADDR + 0x4B] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x4C] = 0x80;
    chip8->RAM[FONT_START_ADDR + 0x4D] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x4E] = 0x80;
    chip8->RAM[FONT_START_ADDR + 0x4F] = 0x80;
}

bool chip8_load_rom(CHIP8 *chip8, char *filename)
{
    /* Loads a given ROM into memory starting at address 0x200.
    0x200 is where user data is stored, eeverything before that is system. */

    FILE *rom = fopen(filename, "rb");
    if (rom)
    {
        if (fread(chip8->RAM + PC_START_ADDR, MAX_RAM - PC_START_ADDR, 1, rom) != 0)
        {
            printf("Really just here to get GCC to shut up about unused return values.\n");
        }

        fclose(rom);

        return true;
    }
    else
    {
        return false;
    }
}

void chip8_execute(CHIP8 *chip8)
{
    // The first and second byte of instruction respectively.
    unsigned char b1 = chip8->RAM[chip8->PC], b2 = chip8->RAM[chip8->PC + 1];

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

    chip8->PC += 2;

    chip8->display_updated = false;

    // CLS (00E0):
    if (b1 == 0x00 && b2 == 0xE0)
    {
        chip8_reset_display(chip8);
    }

    // RET (00EE):
    else if (b1 == 0x00 && b2 == 0xEE)
    {
        chip8->PC = (chip8->RAM[chip8->SP] << 8) | chip8->RAM[chip8->SP + 1];
        chip8->SP -= 2;
    }

    switch (C)
    {
    // JMP NNN (1NNN):
    case 0x01:
        chip8->PC = NNN;
        break;

    // CALL NNN (2NNN):
    case 0x02:
        chip8->SP += 2;
        chip8->RAM[chip8->SP] = chip8->PC >> 8;
        chip8->RAM[chip8->SP + 1] = chip8->PC & 0x00FF;
        chip8->PC = NNN;
        break;

    // SE VX, NN (3XNN):
    case 0x03:
        if (chip8->V[X] == NN)
        {
            chip8->PC += 2;
        }

        break;

    // SNE VX, NN (4XNN):
    case 0x04:
        if (chip8->V[X] != NN)
        {
            chip8->PC += 2;
        }

        break;

    // SE VX, VY (5XY0):
    case 0x05:
        if (chip8->V[X] == chip8->V[Y])
        {
            chip8->PC += 2;
        }

        break;

    // LD VX, NN (6XNN):
    case 0x06:
        chip8->V[X] = NN;
        break;

    // ADD VX, NN (7XNN):
    case 0x07:
        chip8->V[X] += NN;
        break;

    // Bitwise family
    case 0x08:
        switch (N)
        {
        // LD VX, VY (8XY0):
        case 0x00:
            chip8->V[X] = chip8->V[Y];
            break;

        // OR VX, VY (8XY1):
        case 0x01:
            chip8->V[X] |= chip8->V[Y];
            break;

        // AND VX, VY (8XY2):
        case 0x02:
            chip8->V[X] &= chip8->V[Y];
            break;

        // XOR VX, VY (8XY3):
        case 0x03:
            chip8->V[X] ^= chip8->V[Y];
            break;

        // ADD VX, VY (8XY4):
        case 0x04:
            chip8->V[0x0F] = ((int)(chip8->V[X]) + (int)(chip8->V[Y]) > 0xFF) ? 1 : 0;
            chip8->V[X] += chip8->V[Y];
            break;

        // SUB VX, VY (8XY5):
        case 0x05:
            chip8->V[0x0F] = (chip8->V[X] > chip8->V[Y]) ? 1 : 0;
            chip8->V[X] -= chip8->V[Y];
            break;

        // SHR VX {, VY} (8XY6):
        case 0x06:
            //chip8->V[X] = chip8->V[Y];
            chip8->V[0x0F] = chip8->V[X] & 0x01;
            chip8->V[X] >>= 1;
            break;

        // SUBN VX, VY (8XY7):
        case 0x07:
            chip8->V[0x0F] = (chip8->V[Y] > chip8->V[X]) ? 1 : 0;
            chip8->V[X] = chip8->V[Y] - chip8->V[X];
            break;

        // SHL VX {, VY} (8XYE):
        case 0x0E:
            //chip8->V[X] = chip8->V[Y];
            chip8->V[0x0F] = (chip8->V[X] & 0x80) >> 7;
            chip8->V[X] <<= 1;
            break;

        default:
            break;
        }

        break;

    // SNE VX, VY (9XY0):
    case 0x09:
        if (chip8->V[X] != chip8->V[Y])
        {
            chip8->PC += 2;
        }

        break;

    // LD I, NNN (ANNN):
    case 0x0A:
        chip8->I = NNN;
        break;

    // JMP V0, NNN (BNNN):
    case 0x0B:
        chip8->PC = chip8->V[0] + NNN;
        break;

    // RND VX, NN (CXNN):
    case 0x0C:
        chip8->V[X] = (rand() % 256) & NN;
        break;

    // DRW VX, VY, N (DXYN):
    case 0x0D:
        chip8->V[0x0F] = 0;

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                int disp_x = chip8->V[X] + j;
                int disp_y = chip8->V[Y] + i;

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
                bool pixel_on = chip8->display[disp_y][disp_x];
                bool bit = (chip8->RAM[chip8->I + i] >> (7 - j)) & 0x01;

                /* XOR the sprite onto display. 
                If a pixel is erased, set the VF register to 1. */
                chip8->display[disp_y][disp_x] = (pixel_on != bit);
                if (pixel_on && bit)
                {
                    chip8->V[0x0F] = 1;
                }
            }
        }
        chip8->display_updated = true;

        break;

    // Key skip family
    case 0x0E:
        switch (b2)
        {
        // SKP VX (EX9E):
        case 0x9E:
            if (chip8->keypad[chip8->V[X]] == 1)
            {
                chip8->PC += 2;
            }

            break;

        // SKNP VX (EXA1):
        case 0xA1:
            if (chip8->keypad[chip8->V[X]] == 0)
            {
                chip8->PC += 2;
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
            chip8->V[X] = chip8->DT;
            break;

        // LD VX, K (FX0A):
        case 0x0A:
        {
            bool key_released = false;

            for (int i = 0; i < MAX_KEYS; i++)
            {
                if (chip8->keypad[i] == 2)
                {
                    chip8->V[X] = i;
                    key_released = true;
                    break;
                }
            }

            if (!key_released)
            {
                chip8->PC -= 2;
            }

            break;
        }

        // LD DT, VX (FX15):
        case 0x15:
            chip8->DT = chip8->V[X];
            break;

        // LD ST, VX (FX18):
        case 0x18:
            chip8->ST = chip8->V[X];
            break;

        // ADD I, VX (FX1E):
        case 0x1E:
            chip8->I += chip8->V[X];
            break;

        // LD F, VX (FX29):
        case 0x29:
            chip8->I = chip8->V[X] * 0x05;
            break;

        // LD B, VX (FX33):
        case 0x33:
            chip8->RAM[chip8->I] = (chip8->V[X] / 100) % 10;
            chip8->RAM[chip8->I + 1] = (chip8->V[X] / 10) % 10;
            chip8->RAM[chip8->I + 2] = chip8->V[X] % 10;
            break;

        // LD [I], VX (FX55):
        case 0x55:
            for (int r = 0; r <= X; r++)
            {
                chip8->RAM[chip8->I + r] = chip8->V[r];
            }

            break;

        // LD VX, [I] (FX65):
        case 0x65:
            for (int r = 0; r <= X; r++)
            {
                chip8->V[r] = chip8->RAM[chip8->I + r];
            }

            break;

        default:
            break;
        }

        break;

    default:
        break;
    }

    // Any key that was released previous frame gets turned off.
    for (int k = 0; k < MAX_KEYS; k++)
    {
        if (chip8->keypad[k] == 2)
        {
            chip8->keypad[k] = 0;
        }
    }

    chip8_sleep();
}

void chip8_handle_timers(CHIP8 *chip8)
{
    // Decrement timers at a frequency of 60Hz and play sound if needed.
    long freq = CLOCKS_PER_SEC / 60;
    int cycle = clock() % freq;
    if (cycle >= 0 && cycle <= 1000)
    {
        if (chip8->DT > 0)
        {
            chip8->DT--;
        }
        if (chip8->ST > 0)
        {
            chip8->ST--;
            chip8->beep = true;
        }
        else
        {
            chip8->beep = false;
        }
    }
}

void chip8_reset_RAM(CHIP8 *chip8)
{
    for (int addr = 0; addr < MAX_RAM; addr++)
    {
        chip8->RAM[addr] = 0x00;
    }
}

void chip8_reset_keypad(CHIP8 *chip8)
{
    for (int k = 0; k < MAX_KEYS; k++)
    {
        chip8->keypad[k] = 0;
    }
}

void chip8_reset_display(CHIP8 *chip8)
{
    for (int y = 0; y < MAX_HEIGHT; y++)
    {
        for (int x = 0; x < MAX_WIDTH; x++)
        {
            chip8->display[y][x] = false;
        }
    }
}

void chip8_load_instr(CHIP8 *chip8, unsigned int instr)
{
    unsigned char b1 = instr >> 8;
    unsigned char b2 = instr & 0xFF;

    chip8->RAM[PC_START_ADDR] = b1;
    chip8->RAM[PC_START_ADDR + 1] = b2;
}

void chip8_sleep()
{
#ifdef WIN32
    Sleep(1000 / CLOCK_SPEED);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000000000 / CLOCK_SPEED;
    nanosleep(&ts, NULL);
#else
    usleep(1000000 / CLOCK_SPEED);
#endif
}
