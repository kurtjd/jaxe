# JAXE (Just Another XO-CHIP/CHIP-8 Emulator)
|Brix|Space Invaders (In Debug Mode)|
|----|------------------------------|
|<img src = "/screenshots/Brix_CH8.gif?raw=true">|<img src = "/screenshots/Invaders_CH8.gif?raw=true">|

|Black Rainbow|DVN8|
|-------------|----|
|<img src = "/screenshots/Rainbow_CH8.gif?raw=true">|<img src = "/screenshots/DVN8_CH8.gif?raw=true">|

|Super Neat Boy|Chicken Scratch|
|--------------|---------------|
|<img src = "/screenshots/SuperNeatBoy_CH8.gif?raw=true">|<img src = "/screenshots/Chicken_Scratch_CH8.gif?raw=true">|

CHIP-8 was a virtual machine/programming language developed by Joseph Weisbecker for the COSMAC VIP in the 1970s. It was designed with 35 opcodes and resembles assembly language, but was made for the easier development of video games on the VIP.

Today, it is a popular target to be emulated because of its simplicity and charm.

## Features
* Fully implemented instruction set (including S-CHIP and XO-CHIP)
* HI-RES (128x64) mode to support S-CHIP programs
* Dual display buffers to support XO-CHIP programs 
* Accurate delay and sound timers
* Extended sound (though can sometimes be a bit noisy)
* Integrated graphical debugger allowing user to step forward and back through program execution
* Adjustable CPU/timer/refresh frequencies, display scale, colors, and program start address
* Toggle S-CHIP "quirks" for compatibility with a wide variety of ROMs
* Save and load memory dumps
* Unit tests for those writing a C emulator
* Emulator decoupled from any particular graphics/media library allowing for easy embedding into other C programs
* Libretro port

## Technical Info
The original CHIP-8 virtual machine was designed with the following specs:
* 35 opcodes
* 4kb RAM
* 16 8-bit general purpose registers
* 16-bit program counter, stack pointer, and index registers
* 8-bit delay and sound timer registers
* 64x32 monochrome display
* 16-key keypad (0-F)
* Program memory starting at address 0x200

Due to the way CHIP-8 was designed, the "flicker" that happens when sprites are drawn is normal. Games developed for it also rarely made any attempt to cap their frame rate due to the slow hardware of the time hence the need to artificially slow the CPU down on modern emulators.

In the early 90s, Andreas Gustafsson created a port for the HP48 calculator which was eventually superseded by S-CHIP 1.0/1.1 created by Erik Bryntse. The S-CHIP added several features as well as accidentally (or intentionally?) modifying the behavior of several original opcodes:

* 9 new opcodes
* 128x64 HI-RES display
* Persistent storage
* Modified Bnnn, Fx55, Fx65, Dxyn, 8xy6, and 8xyE instructions

With time, it seems the S-CHIP became more popular and many programs were written to work with its various quirks. Thus, JAXE defaults to original S-CHIP design however many of its quirks can be toggled for improved compatibility using the flags in the Options section below.

However, recently John Earnest designed the XO-CHIP extension allowing CHIP-8 programs to take advantage of modern hardware to an extent. This extension adds several more instructions and features including:

* 7 new opcodes
* 16-bit addressing for a total of ~64kb RAM
* Second display buffer allowing for 4 colors instead of the typical 2
* Improved sound support
* Modified Fx75 and Fx85 instructions to allow for 16 user flags instead of typical 8

JAXE currently supports all of these extensions.

It should also be noted that JAXE stores its fonts in memory starting at address **0x0000** followed immediately by large fonts and finally immediately by the stack. Therefore the stack pointer initially points to address **0x00F0**.

## TODO
* Continue to improve sound
* Continue to improve Windows support
* Improve build procedures
* Add more color themes

## Requirements
* SDL2
* SDL2_ttf (for debug mode)
* CMake (for automatic build)

## Build Procedures
### Linux/Windows (MinGW)
`mkdir build && cd build`  
`cmake -B . -DCMAKE_BUILD_TYPE=Release ..`  
`make`

### Windows (non-MinGW)
Unknown at this time. Currently the code uses the POSIX getopt() function to handle command-line arguments. To build without MinGW, remove `#define ALLOW_GETOPTS` from the top of *main.c* which will unfortunately remove command-line arguments until I handle them in a portable way.

## Run
### Linux
`./jaxe [options] <path-to-rom/dump-file>`  
`./test` (for unit tests)

### Windows
If built with MinGW, command line options are available:  
`jaxe.exe [options] <path-to-rom/dump-file>`

Otherwise:  
`jaxe.exe <path-to-rom/dump-file>`  
`test.exe` (for unit tests)

## Options
`-l` Enable legacy mode (for running original CHIP-8 ROMs)  
`-x` Enable XO-CHIP mode  
`-d` Enable debug mode  
`-m` Load dump file instead of ROM  
`-p` Set program start address (in hex)  
`-c` Set CPU frequency (in Hz, value of 0 means uncapped)  
`-t` Set timer frequency (in Hz, value of 0 means uncapped)  
`-r` Set screen refresh frequency (in Hz, value of 0 means uncapped)  
`-s` Set display scale factor  
`-b` Set background color (in hex)  
`-f` Set plane1 color (in hex)  
`-k` Set plane2 color (in hex)  
`-n` Set overlap color (in hex)

Also includes flags for disabling specific S-CHIP "quirks" (which are all enabled by default):

`-0` Disable uninitialized RAM  
`-1` Disable 8xy6/8xyE bug  
`-2` Disable Fx55/Fx65 bug  
`-3` Disable Bnnn bug  
`-4` Disable ban on big sprites being drawn in LO-RES mode  
`-5` Disable display state remaining the same when 00FE/00FF execute (display is cleared with this disabled)  
`-6` Disable sprite clipping  
`-7` Disable collision enumeration  
`-8` Disable collision check with bottom of screen  
`-9` Disable undefined VF after logical OR, AND, XOR (VF is set to 0 with this disabled)


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
* This emulator defaults to S-CHIP mode, which has become more popular since the 90s. Unfortunately, S-CHIP changed the behavior of several instructions and introduced some other quirks, making some programs developed for the original COSMAC VIP not backwards-compatible. If a ROM is not working correctly (especially one written before 1990), try enabling legacy mode with the `-l` flag.
* If running an XO-CHIP ROM, enable XO-CHIP mode with the `-x` flag.
* This emulator defaults to 0x200 as the start address, however some programs assume other defaults (namely, those written for the ETI-660 which default to 0x600). Try to find out what default address the program assumes and set that with the `-p` option.
* If a program is running very slowly, try increasing the CPU speed or even uncapping it (by setting the `-c` option to 0). Some ROMs are developed around an uncapped execution frequency and will run much more smoothly.
* There are many CHIP-8 variants and this emulator does not support all of them. If a ROM still does not work correctly after trying the suggestions above, it may have been written for an unsupported variant and thus will simply not work.

## Libretro port

Libretro port is intended to run under Retroarch on a wide variety of platforms. Compilation:

* Default platform:

```shell
make -f Makefile.libretro
```

* Most platforms

```shell
make -f Makefile.libretro platform=PLATFORM
```

* Android

```
cd jni/
ndk-build
```

Running from command line:

```shell

retroarch -L jaxe_libretro.so  roms/chip8archive/br8kout.ch8
```

I hope to add it to buildbot and official distribution shortly

Default key mapping:

* `0` to `B`
* `1` to `START`
* `2` to `Y`
* `3` to `SELECT`
* `4` to `L` (shoulder)
* `5` to `D-Pad Up`
* `6` to `A`
* `7` to `D-Pad Left`
* `8` to `D-Pad Down`
* `9` to `D-Pad Right`
* `A` to `R` (shoulder)
* `B` to `L2` (trigger)
* `C` to `X`
* `D` to `R2` (trigger)
* `E` to `L3` (thumb)
* `F` to `R3` (thumb)

This is preliminary and subject to change.

Differences and notes compared to standalone version:

* Debug mode is missing
* Userflags are saved as SRAM rather than .uf file
* We use options instead of command line
* CPU frequency can be set only to predefined values. This is a limitation of options interface
* Colors can be chosen only from predefined themes. This is a limitation of options interface
* Setting all quirks to false needs to set all of them manually (there is no equivalent to -x option)
* To cycle through themes or to change CPU frequencies you need to go to options menu
* Loading and storing dumps is done through serialization
* Display scale, pause and exit are handled by frontend
* Audio is resampled in the core and always outputs at 44100

TODOs:

* Add more color themes (same as standalone)
* Investigate if better key mapping is possible
* Allow frontend to peek into "VRAM"
* Uncapped CPU frequency
* Option for PC start address
* Option for timer frequency
* Option for refresh frequency
* Choose resampled audio frequency based on available outputs or make it configurable
* Maybe have retroachievements support?


## Contributing
Anyone is welcome to contribute! I will try to review pull requests as quickly as possible.

## License
JAXE is licensed under the MIT license so you are free to do almost whatever you please with this code (see LICENSE file).

## ROMs
* [CHIP-8 Archive](https://johnearnest.github.io/chip8Archive/)
* [Revival Studio's Chip-8 Program Pack](https://github.com/kripod/chip8-roms)

## References
* [Cowgod's Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
* [Matthew Mikolay's Instruction Set](https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set)
* [HP48-Superchip Investigations](https://github.com/Chromatophore/HP48-Superchip)
* [CHIP-8 extensions and compatibility](https://chip-8.github.io/extensions/)
* [XO-CHIP Specification](https://johnearnest.github.io/Octo/docs/XO-ChipSpecification.html)
* [CHIP-8 Test Suite](https://github.com/Timendus/chip8-test-suite)

## Acknowledgments
* [Kongtext Font by codeman38](https://www.1001fonts.com/kongtext-font.html)
