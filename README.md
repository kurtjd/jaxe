# JACE (Just Another CHIP-8 Emulator)
|Brix|Space Invaders (In Debug Mode)|
|----|------------------------------|
|<img src = "/screenshots/Brix_CH8.gif?raw=true">|<img src = "/screenshots/Invaders_CH8.gif?raw=true">|

CHIP-8 was a virtual machine/programming language developed by Joseph Weisbecker for the COSMAC VIP in the 1970s. It was designed with 35 opcodes and resembles assembly language, but was made for the easier development of video games on the VIP.

Today, it is a popular target to be emulated because of its simplicity and charm.

## Features
* Fully implemented instruction set
* Accurate delay and sound timers
* Graphical display
* Sound
* Graphical debug mode
* Adjustable clock speed, display scale, colors, and program start address
* Unit tests for those writing a C emulator
* Emulator decoupled from any particular graphics/media library allowing for easy embedding into other C programs

## TODO
* Add full S-CHIP support
* Add ETI-660 and HI-RES support
* Allow user to reset emulator
* Allow user to pause in non-debug mode
* Allow user to change clock speed while running
* Allow user to change color while running
* Allow user to create save states (RAM dumps) while in non-debug mode
* Allow user to rewind in debug mode
* Enhance debug display
* Look into support for other CHIP-8 variants
* Add GUI
* Add more test cases
* Test Windows support
* Continue to refactor and optimize code
* Continue to tweak README and add documentation

## Requirements
* SDL2
* SDL2_ttf (for debug mode)

## Build JACE
`mkdir build && cd build`  
`cmake -B. --config Release --target jace ..`  
`make jace`

## Build Test (Debug Must Be Set)
`mkdir build && cd build`  
`cmake -B. --config Debug --target test ..`  
`make test`

## Run
`./jace [options] <path-to-rom>`  
`./test` (for unit tests)

## Options
`-l` Enable legacy mode (for programs that ran on the COSMAC VIP)  
`-d` Enable debug mode (with optional dump file specified, otherwise defaults to jace.dmp)  
`-p` Set program start address (in hex)  
`-c` Set clock speed (in Hz)  
`-s` Set display scale factor  
`-x` Set pixel ON color (in hex)  
`-y` Set pixel OFF color (in hex)

## Controls
**Keyboard (This maps to the key layouts below)**:  
`1` `2` `3` `4`  
`Q` `W` `E` `R`  
`A` `S` `D` `F`  
`Z` `X` `C` `V`

**COSMAC VIP Keypad**:  
`1` `2` `3` `C`  
`4` `5` `6` `D`  
`7` `8` `9` `E`  
`A` `0` `B` `F`

**HP48 Keypad**:  
`7` `8` `9` `/`  
`4` `5` `6` `*`  
`1` `2` `3` `-`  
`0` `.` `_` `+`

## Troubleshooting
* This emulator defaults to S-CHIP mode, which has become more popular since the 90s. Unfortunately, S-CHIP changed the behavior of several instructions, making some programs developed for the original COSMAC VIP not backwards-compatible. If a ROM is not working correctly (especially one written before 1990), try enabling legacy mode with the `-l` flag.
* This emulator defaults to 0x200 as the start address, however some programs assume other defaults (namely, those written for the ETI-660 which default to 0x600). Try to find out what default address the program assumes and set that with the `-p` option.
* There are many CHIP-8 variants and this emulator does not support all of them. If a ROM still does not work correctly after trying the suggestions above, it may have been written for an unsupported variant and thus will simply not work.

## Contributing
Anyone is welcome to contribute! I will try to review pull requests as quickly as possible.

## License
JACE is licensed under the MIT license so you are free to do almost whatever you please with this code (see LICENSE file).

## ROMs
* [Revival Studio's Chip-8 Program Pack](https://github.com/kripod/chip8-roms)

## References
* [Cowgod's Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
* [Matthew Mikolay's Instruction Set](https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set)

## Acknowledgements
* [Kongtext Font by codeman38](https://www.1001fonts.com/kongtext-font.html)
* [J. Th.'s Sound Code](https://stackoverflow.com/a/45002609)
