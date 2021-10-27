#include <stdio.h>
#include <stdbool.h>

bool load_rom(char *filename, unsigned char memory[])
{
    /* Loads a given ROM into memory starting at address 0x200.
    0x200 is where user data is stored, eeverything before that is system. */

    FILE *rom = fopen(filename, "rb");
    if (rom)
    {
        fread(memory + 0x200, 4096 - 0x200, 1, rom);
        return true;
    }
    else
    {
        return false;
    }
}

void load_font(unsigned char memory[])
{
    /* Load hexadecimal font into addresses 0x000-0x04F.
    Each hex character is represented by 5 bytes in memory
    with each bit representing a pixel. */

    // 0:
    memory[0x0] = 0xF0;
    memory[0x1] = 0x90;
    memory[0x2] = 0x90;
    memory[0x3] = 0x90;
    memory[0x4] = 0xF0;
    // 1:
    memory[0x5] = 0x20;
    memory[0x6] = 0x60;
    memory[0x7] = 0x20;
    memory[0x8] = 0x20;
    memory[0x9] = 0x70;
    // 2:
    memory[0xA] = 0xF0;
    memory[0xB] = 0x10;
    memory[0xC] = 0xF0;
    memory[0xD] = 0x80;
    memory[0xE] = 0xF0;
    // 3:
    memory[0xF] = 0xF0;
    memory[0x10] = 0x10;
    memory[0x11] = 0xF0;
    memory[0x12] = 0x10;
    memory[0x13] = 0xF0;
    // 4:
    memory[0x14] = 0x90;
    memory[0x15] = 0x90;
    memory[0x16] = 0xF0;
    memory[0x17] = 0x10;
    memory[0x18] = 0x10;
    // 5:
    memory[0x19] = 0xF0;
    memory[0x1A] = 0x80;
    memory[0x1B] = 0xF0;
    memory[0x1C] = 0x10;
    memory[0x1D] = 0xF0;
    // 6:
    memory[0x1E] = 0xF0;
    memory[0x1F] = 0x80;
    memory[0x20] = 0xF0;
    memory[0x21] = 0x90;
    memory[0x22] = 0xF0;
    // 7:
    memory[0x23] = 0xF0;
    memory[0x24] = 0x10;
    memory[0x25] = 0x20;
    memory[0x26] = 0x40;
    memory[0x27] = 0x40;
    // 8:
    memory[0x28] = 0xF0;
    memory[0x29] = 0x90;
    memory[0x2A] = 0xF0;
    memory[0x2B] = 0x90;
    memory[0x2C] = 0xF0;
    // 9:
    memory[0x2D] = 0xF0;
    memory[0x2E] = 0x90;
    memory[0x2F] = 0xF0;
    memory[0x30] = 0x10;
    memory[0x31] = 0xF0;
    // A:
    memory[0x32] = 0xF0;
    memory[0x33] = 0x90;
    memory[0x34] = 0xF0;
    memory[0x35] = 0x90;
    memory[0x36] = 0x90;
    // B:
    memory[0x37] = 0xE0;
    memory[0x38] = 0x90;
    memory[0x39] = 0xE0;
    memory[0x3A] = 0x90;
    memory[0x3B] = 0xE0;
    // C:
    memory[0x3C] = 0xF0;
    memory[0x3D] = 0x80;
    memory[0x3E] = 0x80;
    memory[0x3F] = 0x80;
    memory[0x40] = 0xF0;
    // D:
    memory[0x41] = 0xE0;
    memory[0x42] = 0x90;
    memory[0x43] = 0x90;
    memory[0x44] = 0x90;
    memory[0x45] = 0xE0;
    // E:
    memory[0x46] = 0xF0;
    memory[0x47] = 0x80;
    memory[0x48] = 0xF0;
    memory[0x49] = 0x80;
    memory[0x4A] = 0xF0;
    // F:
    memory[0x4B] = 0xF0;
    memory[0x4C] = 0x80;
    memory[0x4D] = 0xF0;
    memory[0x4E] = 0x80;
    memory[0x4F] = 0x80;
}

int main()
{
    // Represents 4096 bytes (4KB) of addressable memory.
    unsigned char memory[4096] = {0x00};

    // Represents 16 general-purpose 8-bit registers (V0-VF).
    unsigned char V[16];

    /* Represents the delay timer, sound timer and
    stack pointer 8-bit registers. */
    unsigned char DT = 0, ST = 0, SP = 0;

    // Represents the program counter and index 16-bit registers.
    unsigned int PC = 0x200, I = 0x00;

    // Represents the call stack. CHIP-8 allows a max of 16 nested calls.
    unsigned int stack[16];

    /* Represents a monochrome display of 64x32 pixels.
    A pixel can be either only on or off, no color. */
    bool display[32][64] = {false};

    load_font(memory);

    // Load ROM into memory (for now filename hardcoded).
    if (!load_rom("../roms/my_test.c8", memory))
    {
        fprintf(stderr, "Unable to open ROM file.\n");
        return 1;
    }

    // Execute instructions until none (0x0000) is found.
    while (!(memory[PC] == 0x00 && memory[PC + 1] == 0x00))
    {
        // CLS:
        if (memory[PC] == 0x00 && memory[PC + 1] == 0xE0)
        {
            for (int row = 0; row < 32; row++)
            {
                for (int col = 0; col < 64; col++)
                {
                    display[row][col] = false;
                }
            }
        }

        PC += 2;
    }

    return 0;
}