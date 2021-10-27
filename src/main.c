#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

bool load_rom(char *filename, unsigned char RAM[])
{
    /* Loads a given ROM into memory starting at address 0x200.
    0x200 is where user data is stored, eeverything before that is system. */

    FILE *rom = fopen(filename, "rb");
    if (rom)
    {
        fread(RAM + 0x200, 4096 - 0x200, 1, rom);
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
    /* Load hexadecimal font into addresses 0x000-0x04F.
    Each hex character is represented by 5 bytes in memory
    with each bit representing a pixel. */

    // 0:
    RAM[0x0] = 0xF0;
    RAM[0x1] = 0x90;
    RAM[0x2] = 0x90;
    RAM[0x3] = 0x90;
    RAM[0x4] = 0xF0;
    // 1:
    RAM[0x5] = 0x20;
    RAM[0x6] = 0x60;
    RAM[0x7] = 0x20;
    RAM[0x8] = 0x20;
    RAM[0x9] = 0x70;
    // 2:
    RAM[0xA] = 0xF0;
    RAM[0xB] = 0x10;
    RAM[0xC] = 0xF0;
    RAM[0xD] = 0x80;
    RAM[0xE] = 0xF0;
    // 3:
    RAM[0xF] = 0xF0;
    RAM[0x10] = 0x10;
    RAM[0x11] = 0xF0;
    RAM[0x12] = 0x10;
    RAM[0x13] = 0xF0;
    // 4:
    RAM[0x14] = 0x90;
    RAM[0x15] = 0x90;
    RAM[0x16] = 0xF0;
    RAM[0x17] = 0x10;
    RAM[0x18] = 0x10;
    // 5:
    RAM[0x19] = 0xF0;
    RAM[0x1A] = 0x80;
    RAM[0x1B] = 0xF0;
    RAM[0x1C] = 0x10;
    RAM[0x1D] = 0xF0;
    // 6:
    RAM[0x1E] = 0xF0;
    RAM[0x1F] = 0x80;
    RAM[0x20] = 0xF0;
    RAM[0x21] = 0x90;
    RAM[0x22] = 0xF0;
    // 7:
    RAM[0x23] = 0xF0;
    RAM[0x24] = 0x10;
    RAM[0x25] = 0x20;
    RAM[0x26] = 0x40;
    RAM[0x27] = 0x40;
    // 8:
    RAM[0x28] = 0xF0;
    RAM[0x29] = 0x90;
    RAM[0x2A] = 0xF0;
    RAM[0x2B] = 0x90;
    RAM[0x2C] = 0xF0;
    // 9:
    RAM[0x2D] = 0xF0;
    RAM[0x2E] = 0x90;
    RAM[0x2F] = 0xF0;
    RAM[0x30] = 0x10;
    RAM[0x31] = 0xF0;
    // A:
    RAM[0x32] = 0xF0;
    RAM[0x33] = 0x90;
    RAM[0x34] = 0xF0;
    RAM[0x35] = 0x90;
    RAM[0x36] = 0x90;
    // B:
    RAM[0x37] = 0xE0;
    RAM[0x38] = 0x90;
    RAM[0x39] = 0xE0;
    RAM[0x3A] = 0x90;
    RAM[0x3B] = 0xE0;
    // C:
    RAM[0x3C] = 0xF0;
    RAM[0x3D] = 0x80;
    RAM[0x3E] = 0x80;
    RAM[0x3F] = 0x80;
    RAM[0x40] = 0xF0;
    // D:
    RAM[0x41] = 0xE0;
    RAM[0x42] = 0x90;
    RAM[0x43] = 0x90;
    RAM[0x44] = 0x90;
    RAM[0x45] = 0xE0;
    // E:
    RAM[0x46] = 0xF0;
    RAM[0x47] = 0x80;
    RAM[0x48] = 0xF0;
    RAM[0x49] = 0x80;
    RAM[0x4A] = 0xF0;
    // F:
    RAM[0x4B] = 0xF0;
    RAM[0x4C] = 0x80;
    RAM[0x4D] = 0xF0;
    RAM[0x4E] = 0x80;
    RAM[0x4F] = 0x80;
}

int main()
{
    srand(time(NULL));

    // Represents 4096 bytes (4KB) of addressable memory.
    unsigned char RAM[4096] = {0x00};

    // Represents 16 general-purpose 8-bit registers (V0-VF).
    unsigned char V[16];

    // Delay timer, sound timer and stack pointer 8-bit registers.
    unsigned char DT = 0, ST = 0, SP = 0;

    // Program counter and index 16-bit registers.
    unsigned int PC = 0x200, I = 0x00;

    // The call stack. CHIP-8 allows a max of 16 nested calls.
    unsigned int stack[16];

    /* A monochrome display of 64x32 pixels.
    A pixel can be either only on or off, no color. */
    bool display[32][64] = {false};

    load_font(RAM);

    // Load ROM into memory (for now filename hardcoded).
    if (!load_rom("../roms/my_test.c8", RAM))
    {
        fprintf(stderr, "Unable to open ROM file.\n");
        return 1;
    }

    // Read and execute instructions from memory until none (0x0000) is found.
    while (!(RAM[PC] == 0x00 && RAM[PC + 1] == 0x00))
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

        // CLS:
        if (b1 == 0x00 && b2 == 0xE0)
        {
            for (int row = 0; row < 32; row++)
            {
                for (int col = 0; col < 64; col++)
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
                if ((int)(V[X]) + (int)(V[Y]) > 0xFF)
                {
                    V[0xF] = 1;
                }
                else
                {
                    V[0xF] = 0;
                }

                V[X] += V[Y];
                break;

            // SUB VX, VY
            case 0x05:
                if (V[X] > V[Y])
                {
                    V[0xF] = 1;
                }
                else
                {
                    V[0xF] = 0;
                }

                V[X] -= V[Y];
                break;

            // SHR VX {, VY}
            case 0x06:
                V[0xF] = V[X] & 0x01;
                V[X] >> 1;
                break;

            // SUBN VX, VY
            case 0x07:
                if (V[Y] > V[X])
                {
                    V[0xF] = 1;
                }
                else
                {
                    V[0xF] = 0;
                }

                V[X] = V[Y] - V[X];
                break;

            // SHL VX {, VY}
            case 0x0E:
                V[0xF] = V[X] & 0x80;
                V[X] << 1;
                break;

            default:
                break;
            }

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
            // Do draw stuff
            break;

        // Key skip family
        case 0x0E:
            switch (b2)
            {
            // SKP VX
            case 0x9E:
                // Do key stuff
                break;

            // SKNP VX
            case 0xA1:
                // Do key stuff
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
                V[X] = DT;
                break;

            // LD VX, K
            case 0x0A:
                // Do key stuff
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

    return 0;
}