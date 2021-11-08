# JACE (Just Another CHIP-8 Emulator)
|Brix|Space Invaders (In Debug Mode)|
|----|------------------------------|
|<img src = "/screenshots/Brix_CH8.gif?raw=true">|<img src = "/screenshots/Invaders_CH8.gif?raw=true">|

|Black Rainbow|DVN8|
|-------------|----|
|<img src = "/screenshots/Rainbow_CH8.gif?raw=true">|<img src = "/screenshots/DVN8_CH8.gif?raw=true">|

CHIP-8 was a virtual machine/programming language developed by Joseph Weisbecker for the COSMAC VIP in the 1970s. It was designed with 35 opcodes and resembles assembly language, but was made for the easier development of video games on the VIP.

Today, it is a popular target to be emulated because of its simplicity and charm.

## Features
* Fully implemented instruction set (including S-CHIP)
* HI-RES (128x64) mode
* Accurate delay and sound timers
* Graphical display
* Sound
* Graphical debug mode allowing to step forward and back through program execution
* Adjustable CPU/timer/refresh frequencies, display scale, colors, and program start address
* Toggle S-CHIP "quirks" for compatibility with a wide variety of ROMs
* Save and load memory dumps
* Unit tests for those writing a C emulator
* Emulator decoupled from any particular graphics/media library allowing for easy embedding into other C programs

## TODO
* Add XO-CHIP Support
* Improve Windows Support (command-line arguments and sound issue)
* Improve Build Procedures

## Requirements
* SDL2
* SDL2_ttf (for debug mode)
* CMake (for automatic build)

## Build Procedures
### Linux/Windows (MinGW)
`mkdir build && cd build`  
`cmake -B. --config Release --target jace ..`  
`make jace`

### Windows (non-MinGW)
Unknown at this time. Currently the code uses the POSIX getopt() function to handle command-line arguments. To build without MinGW, remove `#define ALLOW_GETOPTS` from the top of *main.c* which will unfortunately remove command-line arguments until I handle them in a portable way.

### Unit Tests (Debug Must Be Set)
`mkdir build && cd build`  
`cmake -B. --config Debug --target test ..`  
`make test`

## Run
### Linux
`./jace [options] <path-to-rom/dump-file>`  
`./test` (for unit tests)

### Windows
If built with MinGW, command line options are available:  
`jace.exe [options] <path-to-rom/dump-file>`

Otherwise:  
`jace.exe <path-to-rom/dump-file>`

`test.exe` (for unit tests)

## Options
`-x` Enable compatibility mode (disables all S-CHIP quirks but keeps HI-RES and new instructions)  
`-d` Enable debug mode  
`-m` Load dump file instead of ROM  
`-p` Set program start address (in hex)  
`-c` Set CPU frequency (in Hz, value of 0 means uncapped)  
`-t` Set timer frequency (in Hz, value of 0 means uncapped)  
`-r` Set screen refresh frequency (in Hz, value of 0 means uncapped)  
`-s` Set display scale factor  
`-f` Set pixel ON (foreground) color (in hex)  
`-b` Set pixel OFF (background) color (in hex)

Also includes flags for disabling specific S-CHIP "quirks" (which are all enabled by default):

`-0` Disable uninitialized RAM  
`-1` Disable 8xy6/8xyE bug  
`-2` Disable Fx55/Fx65 bug  
`-3` Disable Bnnn bug  
`-4` Allow big sprites to be drawn in LO-RES mode  
`-5` Clear display when 00FE/00FF execute  
`-6` Allow sprite wrapping  
`-7` Disable collision enumeration  
`-8` Disable collision check with bottom of screen


## Controls
### Keyboard (This maps to the key layouts below)
`1` `2` `3` `4`  
`Q` `W` `E` `R`  
`A` `S` `D` `F`  
`Z` `X` `C` `V`

### COSMAC VIP Keypad
`1` `2` `3` `C`  
`4` `5` `6` `D`  
`7` `8` `9` `E`  
`A` `0` `B` `F`

### HP48 Keypad
`7` `8` `9` `/`  
`4` `5` `6` `*`  
`1` `2` `3` `-`  
`0` `.` `_` `+`

### Other Controls
|Key|Action|
|---|------|
|`SPACE`|Pause/Unpause|
|`UP`|Step Forward (DBG Mode Only)|
|`DOWN`|Step Back (DBG Mode Only)|
|`RIGHT`|Increase CPU Speed|
|`LEFT`|Decrease CPU Speed|
|`ENTER`|Save/Create Dump File|
|`BACKSPACE`|Cycle Color Themes|
|`ESC`|Reset Emulator|

## Troubleshooting
* This emulator defaults to S-CHIP mode, which has become more popular since the 90s. Unfortunately, S-CHIP changed the behavior of several instructions and introduced some other quirks, making some programs developed for the original COSMAC VIP not backwards-compatible. If a ROM is not working correctly (especially one written before 1990), try enabling compatibility mode with the `-x` flag.
* If running a program developed for Octo, you should also enable compatibility mode with the `-x` flag since Octo ignores all S-CHIP quirks.
* This emulator defaults to 0x200 as the start address, however some programs assume other defaults (namely, those written for the ETI-660 which default to 0x600). Try to find out what default address the program assumes and set that with the `-p` option.
* If a program is running very slowly, try increasing the CPU speed or even uncapping it (by setting the `-c` option to 0). Some ROMs are developed around an uncapped execution frequency and will run much more smoothly.
* There are many CHIP-8 variants and this emulator does not support all of them. If a ROM still does not work correctly after trying the suggestions above, it may have been written for an unsupported variant and thus will simply not work.

## Contributing
Anyone is welcome to contribute! I will try to review pull requests as quickly as possible.

## License
JACE is licensed under the MIT license so you are free to do almost whatever you please with this code (see LICENSE file).

## ROMs
* [CHIP-8 Archive](https://johnearnest.github.io/chip8Archive/)
* [Revival Studio's Chip-8 Program Pack](https://github.com/kripod/chip8-roms)

## References
* [Cowgod's Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
* [Matthew Mikolay's Instruction Set](https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set)
* [John Earnest's Octo](https://github.com/JohnEarnest/Octo)

## Acknowledgements
* [Kongtext Font by codeman38](https://www.1001fonts.com/kongtext-font.html)
* [J. Th.'s Sound Code](https://stackoverflow.com/a/45002609)
