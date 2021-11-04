#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "chip8.h"

void test_00Cn(CHIP8 *chip8)
{
    // TODO
    (void)chip8;
}

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

    chip8_reset(chip8);
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

    chip8_reset(chip8);
}

void test_00FB(CHIP8 *chip8)
{
    // TODO
    (void)chip8;
}

void test_00FC(CHIP8 *chip8)
{
    // TODO
    (void)chip8;
}

void test_00FD(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x00FD);

    chip8_execute(chip8);

    assert(chip8->exit);

    chip8_reset(chip8);
}

void test_00FE(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x00FE);

    chip8_execute(chip8);

    assert(!chip8->hires);

    chip8_reset(chip8);
}

void test_00FF(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x00FF);

    chip8_execute(chip8);

    assert(chip8->hires);

    chip8_reset(chip8);
}

void test_1nnn(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x1FFF);

    chip8_execute(chip8);

    assert(chip8->PC == 0xFFF);

    chip8_reset(chip8);
}

void test_2nnn(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x2FFF);

    chip8_execute(chip8);

    assert(chip8->SP == (SP_START_ADDR + 2));
    assert(chip8->PC == 0xFFF);
    assert(((chip8->RAM[chip8->SP] << 8) | chip8->RAM[chip8->SP + 1]) == (int)(chip8->pc_start_addr + 2));

    chip8_reset(chip8);
}

void test_3xkk(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x3069);

    chip8->V[0] = 0x69;

    chip8_execute(chip8);

    assert(chip8->PC == (chip8->pc_start_addr + 4));

    chip8->PC = chip8->pc_start_addr;
    chip8->V[0] = 0x42;

    chip8_execute(chip8);

    assert(chip8->PC == (chip8->pc_start_addr + 2));

    chip8_reset(chip8);
}

void test_4xkk(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x4069);

    chip8->V[0] = 0x42;

    chip8_execute(chip8);

    assert(chip8->PC == (chip8->pc_start_addr + 4));

    chip8->PC = chip8->pc_start_addr;
    chip8->V[0] = 0x69;

    chip8_execute(chip8);

    assert(chip8->PC == (chip8->pc_start_addr + 2));

    chip8_reset(chip8);
}

void test_5xy0(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x5690);

    chip8->V[6] = 0x42;
    chip8->V[9] = 0x42;

    chip8_execute(chip8);

    assert(chip8->PC == (chip8->pc_start_addr + 4));

    chip8->PC = chip8->pc_start_addr;
    chip8->V[9] = 0x69;

    chip8_execute(chip8);

    assert(chip8->PC == (chip8->pc_start_addr + 2));

    chip8_reset(chip8);
}

void test_6xkk(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x6069);

    chip8_execute(chip8);

    assert(chip8->V[0] == 0x69);

    chip8_reset(chip8);
}

void test_7xkk(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x7069);

    chip8->V[0] = 0x42;

    chip8_execute(chip8);

    assert(chip8->V[0] == (0xAB));

    chip8_reset(chip8);
}

void test_8xy0(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x8690);

    chip8->V[6] = 0x42;
    chip8->V[9] = 0x69;

    chip8_execute(chip8);

    assert(chip8->V[6] == 0x69);

    chip8_reset(chip8);
}

void test_8xy1(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x8691);

    chip8->V[6] = 0xF0;
    chip8->V[9] = 0x0F;

    chip8_execute(chip8);

    assert(chip8->V[6] == 0xFF);

    chip8_reset(chip8);
}

void test_8xy2(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x8692);

    chip8->V[6] = 0xF0;
    chip8->V[9] = 0x0F;

    chip8_execute(chip8);

    assert(chip8->V[6] == 0x00);

    chip8_reset(chip8);
}

void test_8xy3(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x8693);

    chip8->V[6] = 0xF0;
    chip8->V[9] = 0x0F;

    chip8_execute(chip8);

    assert(chip8->V[6] == 0xFF);

    chip8_reset(chip8);
}

void test_8xy4(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x8694);

    chip8->V[6] = 0x05;
    chip8->V[9] = 0x05;

    chip8_execute(chip8);

    assert(chip8->V[6] == 0x0A);
    assert(chip8->V[0x0F] == 0x00);

    chip8->PC = chip8->pc_start_addr;
    chip8->V[6] = 0xFA;
    chip8->V[9] = 0x07;

    chip8_execute(chip8);

    assert(chip8->V[6] == 0x01);
    assert(chip8->V[0x0F] == 0x01);

    chip8_reset(chip8);
}

void test_8xy5(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x8695);

    chip8->V[6] = 0x0A;
    chip8->V[9] = 0x03;

    chip8_execute(chip8);

    assert(chip8->V[6] == 0x07);
    assert(chip8->V[0x0F] == 0x01);

    chip8->PC = chip8->pc_start_addr;
    chip8->V[6] = 0x02;
    chip8->V[9] = 0x04;

    chip8_execute(chip8);

    assert(chip8->V[6] == 0xFE);
    assert(chip8->V[0x0F] == 0x00);

    chip8_reset(chip8);
}

void test_8xy6(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x8696);

    chip8->V[6] = 0x69;

    chip8_execute(chip8);

    assert(chip8->V[6] == 0x34);
    assert(chip8->V[0x0F] == 0x01);

    chip8->PC = chip8->pc_start_addr;
    chip8->V[6] = 0x42;

    chip8_execute(chip8);

    assert(chip8->V[6] == 0x21);
    assert(chip8->V[0x0F] == 0x00);

    chip8_reset(chip8);
}

void test_8xy7(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x8697);

    chip8->V[6] = 0x03;
    chip8->V[9] = 0x0A;

    chip8_execute(chip8);

    assert(chip8->V[6] == 0x07);
    assert(chip8->V[0x0F] == 0x01);

    chip8->PC = chip8->pc_start_addr;
    chip8->V[6] = 0x04;
    chip8->V[9] = 0x03;

    chip8_execute(chip8);

    assert(chip8->V[6] == 0xFF);
    assert(chip8->V[0x0F] == 0x00);

    chip8_reset(chip8);
}

void test_8xyE(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x869E);

    chip8->V[6] = 0x69;

    chip8_execute(chip8);

    assert(chip8->V[6] == 0xD2);
    assert(chip8->V[0x0F] == 0x00);

    chip8->PC = chip8->pc_start_addr;
    chip8->V[6] = 0xF0;

    chip8_execute(chip8);

    assert(chip8->V[6] == 0xE0);
    assert(chip8->V[0x0F] == 0x01);

    chip8_reset(chip8);
}

void test_9xy0(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0x9690);

    chip8->V[6] = 0x42;
    chip8->V[9] = 0x69;

    chip8_execute(chip8);

    assert(chip8->PC == (chip8->pc_start_addr + 4));

    chip8->PC = chip8->pc_start_addr;
    chip8->V[6] = 0x69;
    chip8->V[9] = 0x69;

    chip8_execute(chip8);

    assert(chip8->PC == (chip8->pc_start_addr + 2));

    chip8_reset(chip8);
}

void test_Annn(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0xADAD);

    chip8_execute(chip8);

    assert(chip8->I == 0xDAD);

    chip8_reset(chip8);
}

void test_Bnnn(CHIP8 *chip8)
{
    /*chip8_load_instr(chip8, 0xBBAD);

    chip8->V[0] = 0x69;

    chip8_execute(chip8);

    assert(chip8->PC == 0xC16);*/

    chip8_load_instr(chip8, 0xBBAD);

    chip8->V[0xB] = 0x69;

    chip8_execute(chip8);

    assert(chip8->PC == 0xC16);

    chip8_reset(chip8);
}

void test_Cxkk(CHIP8 *chip8)
{
    // This instruction generates a random byte, thus is hard to test.
    chip8_reset(chip8);
}

void test_Dxyn(CHIP8 *chip8)
{
    // TODO: Redo for S-CHIP functionality.

    chip8_load_instr(chip8, 0xD693);
    chip8->hires = true;

    // 3x3 sprite in top left corner of display.
    chip8->display[0][0] = 1;
    chip8->display[1][0] = 1;
    chip8->display[2][0] = 1;
    chip8->display[0][1] = 1;
    chip8->display[1][1] = 1;
    chip8->display[2][1] = 1;
    chip8->display[0][2] = 1;
    chip8->display[1][2] = 1;
    chip8->display[2][2] = 1;

    // 3x3 sprite in memory.
    chip8->RAM[0x269] = 0xE0;
    chip8->RAM[0x26A] = 0xE0;
    chip8->RAM[0x26B] = 0xE0;

    chip8->I = 0x269;
    chip8->V[6] = 1;
    chip8->V[9] = 1;

    chip8_execute(chip8);

    assert(chip8->V[0x0F] == 1);
    assert(!chip8->display[1][1]);
    assert(!chip8->display[1][2]);
    assert(chip8->display[1][3]);
    assert(!chip8->display[2][1]);
    assert(!chip8->display[2][2]);
    assert(chip8->display[2][3]);
    assert(chip8->display[3][1]);
    assert(chip8->display[3][2]);
    assert(chip8->display[3][3]);

    chip8_reset(chip8);
}

void test_Ex9E(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0xE69E);

    chip8->V[6] = 0xA;
    chip8->keypad[0xA] = 1;

    chip8_execute(chip8);

    assert(chip8->PC == (chip8->pc_start_addr + 4));

    chip8->PC = chip8->pc_start_addr;
    chip8->keypad[0xA] = 0;

    chip8_execute(chip8);

    assert(chip8->PC == (chip8->pc_start_addr + 2));

    chip8_reset(chip8);
}

void test_ExA1(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0xE6A1);

    chip8->V[6] = 0xA;
    chip8->keypad[0xA] = 0;

    chip8_execute(chip8);

    assert(chip8->PC == (chip8->pc_start_addr + 4));

    chip8->PC = chip8->pc_start_addr;
    chip8->keypad[0xA] = 1;

    chip8_execute(chip8);

    assert(chip8->PC == (chip8->pc_start_addr + 2));

    chip8_reset(chip8);
}

void test_Fx07(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0xF007);

    chip8->DT = 0x42;

    chip8_execute(chip8);

    assert(chip8->V[0] == 0x42);

    chip8_reset(chip8);
}

void test_Fx0A(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0xF00A);

    chip8->keypad[0xA] = 1;

    chip8_execute(chip8);

    assert(chip8->PC == chip8->pc_start_addr);

    chip8->keypad[0xA] = 2;

    chip8_execute(chip8);

    assert(chip8->PC == chip8->pc_start_addr + 2 && chip8->V[0] == 0xA);

    chip8_reset(chip8);
}

void test_Fx15(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0xF015);

    chip8->V[0] = 0x69;

    chip8_execute(chip8);

    assert(chip8->DT == 0x69);

    chip8_reset(chip8);
}

void test_Fx18(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0xF018);

    chip8->V[0] = 0x69;

    chip8_execute(chip8);

    assert(chip8->ST == 0x69);

    chip8_reset(chip8);
}

void test_Fx1E(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0xF01E);

    chip8->I = 1;
    chip8->V[0] = 2;

    chip8_execute(chip8);

    assert(chip8->I == 3);

    chip8_reset(chip8);
}

void test_Fx29(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0xF029);

    chip8->V[0] = 0xA;

    chip8_execute(chip8);

    assert(chip8->I == (FONT_START_ADDR + 50));

    chip8_reset(chip8);
}

void test_Fx30(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0xF030);

    chip8->V[0] = 0x6;

    chip8_execute(chip8);

    assert(chip8->I == (BIG_FONT_START_ADDR + 60));

    chip8_reset(chip8);
}

void test_Fx33(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0xF033);

    chip8->I = 0x4;
    chip8->V[0] = 169;

    chip8_execute(chip8);

    assert(chip8->RAM[chip8->I] == 1);
    assert(chip8->RAM[chip8->I + 1] == 6);
    assert(chip8->RAM[chip8->I + 2] == 9);

    chip8->PC = chip8->pc_start_addr;
    chip8->V[0] = 69;

    chip8_execute(chip8);

    assert(chip8->RAM[chip8->I] == 0);
    assert(chip8->RAM[chip8->I + 1] == 6);
    assert(chip8->RAM[chip8->I + 2] == 9);

    chip8->PC = chip8->pc_start_addr;
    chip8->V[0] = 9;

    chip8_execute(chip8);

    assert(chip8->RAM[chip8->I] == 0);
    assert(chip8->RAM[chip8->I + 1] == 0);
    assert(chip8->RAM[chip8->I + 2] == 9);

    chip8_reset(chip8);
}

void test_Fx55(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0xF255);

    chip8->V[0] = 0x69;
    chip8->V[1] = 0x42;
    chip8->V[2] = 0xAB;

    chip8_execute(chip8);

    assert(chip8->RAM[chip8->I] == 0x69);
    assert(chip8->RAM[chip8->I + 1] == 0x42);
    assert(chip8->RAM[chip8->I + 2] == 0xAB);

    chip8_reset(chip8);
}

void test_Fx65(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0xF265);

    chip8->I = 0xBAD;
    chip8->RAM[chip8->I] = 0x69;
    chip8->RAM[chip8->I + 1] = 0x42;
    chip8->RAM[chip8->I + 2] = 0xAB;

    chip8_execute(chip8);

    assert(chip8->V[0] == 0x69);
    assert(chip8->V[1] == 0x42);
    assert(chip8->V[2] == 0xAB);

    chip8_reset(chip8);
}

void test_Fx75_Fx85(CHIP8 *chip8)
{
    chip8_load_instr(chip8, 0xF275);

    char tmp_file[] = "uf_save_test.ch8.uf";
    chip8->V[0] = 0xB;
    chip8->V[1] = 0xA;
    chip8->V[2] = 0xD;
    sprintf(chip8->UF_path, "%s", tmp_file);

    chip8_execute(chip8);
    chip8_reset(chip8);

    chip8_load_instr(chip8, 0xF285);
    sprintf(chip8->UF_path, "%s", tmp_file);
    chip8_execute(chip8);
    remove(tmp_file);

    assert(chip8->V[0] == 0xB);
    assert(chip8->V[1] == 0xA);
    assert(chip8->V[2] == 0xD);

    chip8_reset(chip8);
}

int main()
{
    // The test machine.
    CHIP8 chip8;
    chip8_init(&chip8, false, CLOCK_SPEED_DEFAULT, PC_START_ADDR_DEFAULT);

    /* All tests follow similar pattern:
            * Load instruction into RAM
            * Modify some data to be tested on
            * Execute instruction
            * Check result
            * Reset the machine */
    test_00Cn(&chip8);
    test_00FE(&chip8);
    test_00E0(&chip8);
    test_00EE(&chip8);
    test_00FB(&chip8);
    test_00FC(&chip8);
    test_00FD(&chip8);
    test_00FE(&chip8);
    test_00FF(&chip8);
    test_1nnn(&chip8);
    test_2nnn(&chip8);
    test_3xkk(&chip8);
    test_4xkk(&chip8);
    test_5xy0(&chip8);
    test_6xkk(&chip8);
    test_7xkk(&chip8);
    test_8xy0(&chip8);
    test_8xy1(&chip8);
    test_8xy2(&chip8);
    test_8xy3(&chip8);
    test_8xy4(&chip8);
    test_8xy5(&chip8);
    test_8xy6(&chip8);
    test_8xy7(&chip8);
    test_8xyE(&chip8);
    test_9xy0(&chip8);
    test_Annn(&chip8);
    test_Bnnn(&chip8);
    test_Cxkk(&chip8);
    //test_Dxyn(&chip8);
    test_Ex9E(&chip8);
    test_ExA1(&chip8);
    test_Fx07(&chip8);
    test_Fx0A(&chip8);
    test_Fx15(&chip8);
    test_Fx18(&chip8);
    test_Fx1E(&chip8);
    test_Fx29(&chip8);
    test_Fx30(&chip8);
    test_Fx33(&chip8);
    test_Fx55(&chip8);
    test_Fx65(&chip8);
    test_Fx75_Fx85(&chip8);

    printf("All tests pass!\n");

    return 0;
}
