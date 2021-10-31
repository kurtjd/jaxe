![JACE Pong](/screenshots/jace_pong.png?raw=true)
![JACE Tetris](/screenshots/jace_tetris.png?raw=true)
![JACE Invaders](/screenshots/jace_invaders.png?raw=true)
![JACE Breakout](/screenshots/jace_breakout.png?raw=true)

Just Another CHIP-8 Emulator
============================
A basic CHIP-8 emulator written in C and SDL to practice emulator dev.

* Currently supports all basic instructions, display, and keyboard input.

TODO
=====
* Fix timers
* Generate sound programatically
* Add misc features
* Allow user to change flags
* Add more tests
* Add debug mode

Requirements
============
* SDL2
* SDL2_Mixer (for sound)

Build
=====
`mkdir build && cd build`

`cmake -B. --config Release ..`

`make jace`

For tests: `make test`

Run
===
`./jace <path-to-rom>`

ROMs
====
* [Revival Studio's Chip-8 Program Pack](https://github.com/kripod/chip8-roms)

References
==========
* [Cowgod's Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)