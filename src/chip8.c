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
    // Seed for the RND instruction.
    srand(time(NULL));

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
    // 0:
    chip8->RAM[FONT_START_ADDR + 0x00] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x01] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x02] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x03] = 0x90;
    chip8->RAM[FONT_START_ADDR + 0x04] = 0xF0;

    // 1:
    chip8->RAM[FONT_START_ADDR + 0x05] = 0x20;
    chip8->RAM[FONT_START_ADDR + 0x06] = 0x60;
    chip8->RAM[FONT_START_ADDR + 0x07] = 0x20;
    chip8->RAM[FONT_START_ADDR + 0x08] = 0x20;
    chip8->RAM[FONT_START_ADDR + 0x09] = 0x70;

    // 2:
    chip8->RAM[FONT_START_ADDR + 0x0A] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x0B] = 0x10;
    chip8->RAM[FONT_START_ADDR + 0x0C] = 0xF0;
    chip8->RAM[FONT_START_ADDR + 0x0D] = 0x80;
    chip8->RAM[FONT_START_ADDR + 0x0E] = 0xF0;

    // 3:
    chip8->RAM[FONT_START_ADDR + 0x0F] = 0xF0;
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
    FILE *rom = fopen(filename, "rb");
    if (rom)
    {
        size_t fr = fread(chip8->RAM + PC_START_ADDR,
                          MAX_RAM - PC_START_ADDR,
                          1,
                          rom);
        (void)fr; // Just to suppress fread unused return value warning.

        fclose(rom);

        return true;
    }

    return false;
}

void chip8_execute(CHIP8 *chip8)
{
    /* Fetch */
    // The first and second byte of instruction respectively.
    unsigned char b1 = chip8->RAM[chip8->PC], b2 = chip8->RAM[chip8->PC + 1];

    /* Decode */
    // The code (first 4 bits) of instruction.
    unsigned char c = b1 >> 4;

    // The last 12 bits of instruction.
    unsigned int nnn = ((b1 & 0xF) << 8) | b2;

    // The last 4 bits of instruction.
    unsigned char n = b2 & 0xF;

    // The last 4 bits of first byte of instruction.
    unsigned char x = b1 & 0xF;

    // The first 4 bits of second byte of instruction.
    unsigned char y = b2 >> 4;

    // The last 8 bits of instruction.
    unsigned char kk = b2;

    /* Immediately set PC to next instruction
    after fetching and decoding the current one. */
    chip8->PC += 2;
    chip8->display_updated = false;

    /* Execute */
    switch (c)
    {
    case 0x00:
        switch (b2)
        {
        /* CLS (00E0)
           Clear the display. */
        case 0xE0:
            chip8_reset_display(chip8);
            break;

        /* RET (00EE):
           Return from a subroutine. */
        case 0xEE:
            chip8->PC = (chip8->RAM[chip8->SP] << 8);
            chip8->PC |= chip8->RAM[chip8->SP + 1];
            chip8->SP -= 2;
            break;
        }

        break;

    /* JP addr (1nnn)
       Jump to location nnn. */
    case 0x01:
        chip8->PC = nnn;
        break;

    /* CALL addr (2nnn)
       Call subroutine at nnn. */
    case 0x02:
        chip8->SP += 2;
        chip8->RAM[chip8->SP] = chip8->PC >> 8;
        chip8->RAM[chip8->SP + 1] = chip8->PC & 0x00FF;
        chip8->PC = nnn;
        break;

    /* SE Vx, byte (3xkk)
       Skip next instruction if Vx = kk. */
    case 0x03:
        if (chip8->V[x] == kk)
        {
            chip8->PC += 2;
        }

        break;

    /* SNE Vx, byte (4xkk)
       Skip next instruction if Vx != kk. */
    case 0x04:
        if (chip8->V[x] != kk)
        {
            chip8->PC += 2;
        }

        break;

    /* SE Vx, Vy (5xy0)
       Skip next instruction if Vx = Vy. */
    case 0x05:
        if (chip8->V[x] == chip8->V[y])
        {
            chip8->PC += 2;
        }

        break;

    /* LD Vx, byte (6xkk)
       Set Vx = kk. */
    case 0x06:
        chip8->V[x] = kk;
        break;

    /* ADD Vx, byte (7xkk)
       Set Vx = Vx + kk. */
    case 0x07:
        chip8->V[x] += kk;
        break;

    case 0x08:
        switch (n)
        {
        /* LD Vx, Vy (8xy0)
           Set Vx = Vy. */
        case 0x00:
            chip8->V[x] = chip8->V[y];
            break;

        /* OR Vx, Vy (8xy1)
           Set Vx = Vx OR Vy. */
        case 0x01:
            chip8->V[x] |= chip8->V[y];
            break;

        /* AND Vx, Vy (8xy2)
           Set Vx = Vx AND Vy. */
        case 0x02:
            chip8->V[x] &= chip8->V[y];
            break;

        /* XOR Vx, Vy (8xy3)
           Set Vx = Vx XOR Vy. */
        case 0x03:
            chip8->V[x] ^= chip8->V[y];
            break;

        /* ADD Vx, Vy (8xy4)
           Set Vx = Vx + Vy, set VF = carry. */
        case 0x04:
            chip8->V[0x0F] = ((chip8->V[x] + chip8->V[y]) > 0xFF) ? 1 : 0;
            chip8->V[x] += chip8->V[y];
            break;

        /* SUB Vx, Vy (8xy5)
           Set Vx = Vx - Vy, set VF = NOT borrow. */
        case 0x05:
            chip8->V[0x0F] = (chip8->V[x] > chip8->V[y]) ? 1 : 0;
            chip8->V[x] -= chip8->V[y];
            break;

        /* SHR Vx {, Vy} (8xy6)
           Set Vx = Vx SHR 1. */
        case 0x06:
            //chip8->V[x] = chip8->V[y];
            chip8->V[0x0F] = chip8->V[x] & 0x01;
            chip8->V[x] >>= 1;
            break;

        /* SUBN Vx, Vy (8xy7)
           Set Vx = Vy - Vx, set VF = NOT borrow. */
        case 0x07:
            chip8->V[0x0F] = (chip8->V[y] > chip8->V[x]) ? 1 : 0;
            chip8->V[x] = chip8->V[y] - chip8->V[x];
            break;

        /* SHL Vx {, Vy} (8xyE)
           Set Vx = Vx SHL 1. */
        case 0x0E:
            //chip8->V[x] = chip8->V[y];
            chip8->V[0x0F] = (chip8->V[x] & 0x80) >> 7;
            chip8->V[x] <<= 1;
            break;
        }

        break;

    /* SNE Vx, Vy (9xy0)
       Skip next instruction if Vx != Vy. */
    case 0x09:
        if (chip8->V[x] != chip8->V[y])
        {
            chip8->PC += 2;
        }

        break;

    /* LD I, addr (Annn)
       Set I = nnn. */
    case 0x0A:
        chip8->I = nnn;
        break;

    /* JMP V0, addr (Bnnn)
       Jump to location nnn + V0. */
    case 0x0B:
        chip8->PC = chip8->V[0] + nnn;
        break;

    /* RND Vx, byte (Cxkk)
       Set Vx = random byte AND kk. */
    case 0x0C:
        chip8->V[x] = (rand() % 256) & kk;
        break;

    /* DRW Vx, Vy, n (Dxyn):
       Display n-byte sprite starting at memory location I at (Vx, Vy),
       set VF = collision. */
    case 0x0D:
        chip8->V[0x0F] = 0;

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                int disp_x = chip8->V[x] + j;
                int disp_y = chip8->V[y] + i;

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

    case 0x0E:
        switch (b2)
        {
        /* SKP Vx (Ex9E)
           Skip next instruction if key with the value of Vx is pressed. */
        case 0x9E:
            if (chip8->keypad[chip8->V[x]] == KEY_DOWN)
            {
                chip8->PC += 2;
            }

            break;

        /* SKNP Vx (ExA1)
           Skip next instruction if key with the value of Vx is not pressed. */
        case 0xA1:
            if (chip8->keypad[chip8->V[x]] == KEY_UP)
            {
                chip8->PC += 2;
            }

            break;
        }

        break;

    case 0x0F:
        switch (b2)
        {
        /* LD Vx, DT (Fx07)
           Set Vx = delay timer value. */
        case 0x07:
            chip8->V[x] = chip8->DT;
            break;

        /* LD Vx, K (Fx0A)
           Wait for a key press, store the value of the key in Vx. */
        case 0x0A:
        {
            bool key_released = false;

            for (int i = 0; i < MAX_KEYS; i++)
            {
                if (chip8->keypad[i] == KEY_RELEASED)
                {
                    chip8->V[x] = i;
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

        /* LD DT, Vx (Fx15)
           Set delay timer = Vx. */
        case 0x15:
            chip8->DT = chip8->V[x];
            break;

        /* LD ST, Vx (Fx18)
           Set sound timer = Vx. */
        case 0x18:
            chip8->ST = chip8->V[x];
            break;

        /* ADD I, Vx (Fx1E):
           Set I = I + Vx. */
        case 0x1E:
            chip8->I += chip8->V[x];
            break;

        /* LD F, Vx (Fx29)
           Set I = location of sprite for digit Vx. */
        case 0x29:
            chip8->I = chip8->V[x] * 0x05;
            break;

        /* LD B, Vx (Fx33)
           Store BCD representation of Vx in memory locations:
           I, I+1, and I+2. */
        case 0x33:
            chip8->RAM[chip8->I] = (chip8->V[x] / 100) % 10;
            chip8->RAM[chip8->I + 1] = (chip8->V[x] / 10) % 10;
            chip8->RAM[chip8->I + 2] = chip8->V[x] % 10;
            break;

        /* LD [I], Vx (Fx55)
           Fx55 - LD [I], Vx */
        case 0x55:
            for (int r = 0; r <= x; r++)
            {
                chip8->RAM[chip8->I + r] = chip8->V[r];
            }

            break;

        /* LD Vx, [I] (Fx65)
           Read registers V0 through Vx from memory starting at location I. */
        case 0x65:
            for (int r = 0; r <= x; r++)
            {
                chip8->V[r] = chip8->RAM[chip8->I + r];
            }

            break;
        }

        break;
    }

    // Any key that was released previous frame gets turned off.
    for (int k = 0; k < MAX_KEYS; k++)
    {
        if (chip8->keypad[k] == KEY_RELEASED)
        {
            chip8->keypad[k] = KEY_UP;
        }
    }

    chip8_sleep();
}

void chip8_handle_timers(CHIP8 *chip8)
{
    long freq = CLOCKS_PER_SEC / 60;
    int cycle = clock() % freq;

    // This whole thing will be modified in the future...
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

void chip8_reset_keypad(CHIP8 *chip8)
{
    for (int k = 0; k < MAX_KEYS; k++)
    {
        chip8->keypad[k] = KEY_UP;
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

void chip8_load_instr(CHIP8 *chip8, unsigned int instr)
{
    unsigned char b1 = instr >> 8;
    unsigned char b2 = instr & 0xFF;

    chip8->RAM[PC_START_ADDR] = b1;
    chip8->RAM[PC_START_ADDR + 1] = b2;
}
