# CHIP-8 Interpreter

A CHIP-8 Interepreter written in C as a warm up to writing more advanced emulators down the line. 

## How to install

### Prerequisites 

You need a C compiler, Make, and [SDL2](https://www.libsdl.org/) installed to compile this project.

Debian/Ubuntu: 
```Shell
apt install libsdl2-dev 
```

Arch: 
```Shell
pacman -S sdl2
```

Void Linux: 
```Shell
xbps-install SDL2-devel
```

Fedora/RHEL: 
```Shell
yum install SDL2-devel  
```

### Building from source:
```Shell
git clone https://github.com/SebSherry/chip8.git
cd chip8
make
```

## Usage
```Shell
./chip8 [OPTIONS] <ROM>

Options:
    -d, --debug            Run the debugger
    -s, --scale [SCALE]    Set window scale (Default 10, minimum 1)
    -f [COLOUR]            Set Foreground colour (See Below)
    -b [COLOUR]            Set Background colour (See Below)
    -c, --cycles [CYCLES]  Target CPU cycles per second

Available Colours:
    1 Black
    2 Red
    3 Dark Red
    4 Magenta
    5 Lavender
    6 Green
    7 Dark Green
    8 Yellow
    9 Gold
    10 Orange
    11 Sage
    12 Blue
    13 Sky Blue
    14 Dark Blue
    15 Turqiouse
    16 White
```

## Features
- Full CHIP-8 Support
- Configurable Colours
- Configurable Speed (in Hz)
- Window Scaling
- Debugger 

## Acknowledgements 
Tobias V. Langhoff's [Guide to making a CHIP-8 emulator](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) is a fantastic resource for learning how the CHIP-8 system actually works.
The explaination of the quirks for various instructions was very helpful. If you're planning to write your own interpreter I highly recommend this resource.

Austin Morlan's [Building a CHIP-8 Emulator [C++]](https://austinmorlan.com/posts/chip8_emulator/) was a great reference implementation. 

Timedus' [CHIP-8 Test Suite](https://github.com/Timendus/chip8-test-suite) is a must for anyone writing a CHIP-8 Interpreter. This test rom exposed more than a few small bugs during development. 
Their own implementaion [Silicon8](https://github.com/Timendus/silicon8) was another great reference that helped me pass that pesky display wait test!

## License
This software is licensed under the MIT License. 

See the [LICENSE](LICENSE)