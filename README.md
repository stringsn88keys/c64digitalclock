# C64 Digital Clock

A PETSCII digital clock for the Commodore 64, written in C using the cc65 compiler.

## Features

- Large 7-row PETSCII digits with rounded corners using triangle characters
- Displays time in HH:MM:SS format
- Reads time from the C64's built-in jiffy clock
- Cyan digits on a blue background
- Updates only when seconds change (efficient rendering)
- Press any key to exit

## Requirements

- [cc65](https://cc65.github.io/) - The cc65 cross-compiler suite
- A Commodore 64 or emulator (such as VICE) to run the program

## Building

The build scripts expect cc65 to be installed in a sibling directory (`../cc65`).

### macOS/Linux

```bash
./build.sh
```

### Windows (PowerShell)

```powershell
.\build.ps1
```

Both scripts compile `clock.c` and produce `clock.prg`.

## Running

Load the resulting `clock.prg` file in your C64 emulator or transfer it to real hardware.

In VICE:
```
File -> Smart attach Disk/Tape/Cartridge... -> select clock.prg
```

Or from the VICE command line:
```bash
x64 clock.prg
```

## Technical Details

- Uses the C64's jiffy clock at memory locations $A0-$A2 (TI in BASIC)
- Assumes NTSC timing (60 jiffies per second)
- Renders directly to screen memory at $0400
- Sets color RAM at $D800 for digit coloring
