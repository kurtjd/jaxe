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
* Add sound
* Add misc features
* Allow user to change flags
* Add more tests
* Add debug mode

Requirements
============
* SDL

Build
=====
`mkdir build && cd build`

`cmake -B. --config Release ..`

`make jace`

Run
===
`./jace <path-to-rom>`
