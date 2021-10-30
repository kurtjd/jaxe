#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "chip8.h"

void test_00E0(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x00E0);

    chip8->display[0][0] = true;
    chip8->display[MAX_HEIGHT / 2][MAX_WIDTH / 2] = true;
    chip8->display[MAX_HEIGHT - 1][0] = true;
    chip8->display[0][MAX_WIDTH - 1] = true;
    chip8->display[MAX_HEIGHT - 1][MAX_WIDTH - 1] = true;

    chip8_execute(chip8);

    for (int i = 0; i < MAX_HEIGHT; i++)
    {
        for (int j = 0; j < MAX_WIDTH; j++)
        {
            assert(chip8->display[i][j] == false);
        }
    }

    chip8_init(chip8);
}

void test_00EE(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x00EE);

    chip8->SP = SP_START_ADDR + 5;
    chip8->RAM[chip8->SP] = 0x0D;
    chip8->RAM[chip8->SP + 1] = 0xAD;

    chip8_execute(chip8);

    assert(chip8->SP == (SP_START_ADDR + 3));
    assert(chip8->PC == 0xDAD);

    chip8_init(chip8);
}

int main()
{
    // The test machine.
    CHIP8 chip8;
    chip8_init(&chip8);

    /* All tests follow similar pattern:
            * Load instruction into RAM
            * Modify some data to be tested on
            * Execute instruction
            * Check result
            * Reset the machine */
    test_00E0(&chip8);
    test_00EE(&chip8);

    printf("All tests pass!\n");

    return 0;
}