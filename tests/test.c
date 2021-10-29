#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "chip8.h"

void test_00E0(CHIP8 *chip8)
{
    chip8->RAM[PC_START_ADDR] = 0x00;
    chip8->RAM[PC_START_ADDR + 1] = 0xE0;
    chip8->display[0][0] = true;
    chip8_execute(chip8);
    assert(chip8->display[0][0] == false);
    chip8_init(chip8);
}

int main()
{
    CHIP8 chip8;
    chip8_init(&chip8);

    test_00E0(&chip8);

    printf("All tests pass!\n");

    return 0;
}