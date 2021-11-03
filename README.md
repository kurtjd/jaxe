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

## TODO
* Add full S-CHIP support
* Look into support for other CHIP-8 variants
* Add GUI
* Add more test cases
* Test Windows support
* Continue to refactor and optimize code

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
`-s` Set display scale factor  
`-p` Set program start address (in hex)  
`-c` Set clock speed (in Hz)  
`-x` Set pixel ON color (in hex)  
`-y` Set pixel OFF color (in hex)

## Controls
**Keyboard**:  
`1` `2` `3` `4`  
`Q` `W` `E` `R`  
`A` `S` `D` `F`  
`Z` `X` `C` `V`

**Original Hexpad**:  
`1` `2` `3` `C`  
`4` `5` `6` `D`  
`7` `8` `9` `E`  
`A` `0` `B` `F`

## ROMs
* [Revival Studio's Chip-8 Program Pack](https://github.com/kripod/chip8-roms)

## References
* [Cowgod's Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
* [Matthew Mikolay's Instruction Set](https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set)

## Acknowledgements
* [Kongtext Font by codeman38](https://www.1001fonts.com/kongtext-font.html)
* [J. Th.'s Sound Code](https://stackoverflow.com/a/45002609)
