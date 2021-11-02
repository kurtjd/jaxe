![JACE Pong](/screenshots/jace_pong.png?raw=true)
![JACE Tetris](/screenshots/jace_tetris.png?raw=true)
![JACE Invaders](/screenshots/jace_invaders.png?raw=true)
![JACE Breakout](/screenshots/jace_breakout.png?raw=true)

Just Another CHIP-8 Emulator
============================
A basic CHIP-8 emulator written in C and SDL to practice emulator dev.

* Currently supports all basic instructions, display, sound, and keyboard input.

TODO
=====
* Add full S-CHIP support
* Look into support for other CHIP-8 variants
* Add GUI
* Add more test cases
* Test Windows support
* Continue to refactor and optimize code

Requirements
============
* SDL2
* SDL2_ttf (for debug mode)

Build
=====
`mkdir build && cd build`

`cmake -B. --config Release ..`

`make jace`

Run
===
`./jace [options] <path-to-rom>`

Options
=======
* `-l` Legacy mode (for programs that ran on the COSMAC VIP)

ROMs
====
* [Revival Studio's Chip-8 Program Pack](https://github.com/kripod/chip8-roms)

References
==========
* [Cowgod's Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
* [Matthew Mikolay's Instruction Set](https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set)

Acknowledgements
================
* [Kongtext Font by codeman38](https://www.1001fonts.com/kongtext-font.html)