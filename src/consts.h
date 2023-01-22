#ifndef CONSTS_H_
#define CONSTS_H_

// Screen Constants
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)
#define DEFAULT_SCALE 10
#define MINIMUM_SCALE 1
#define PITCH 256

// System constants
#define NUM_OF_INSTRUCTIONS 34
#define NUM_OF_REGISTERS 16
#define FONTSET_SIZE 80
#define FONT_START_MEMORY_ADDR 0x50
#define ROM_START_MEMORY_ADDR 0x200 

// Colours 
#define BLACK 0
#define RED 0xFF0000FF
#define DARK_RED 0x5C0505FF
#define MAGENTA 0xFB08F7FF
#define LAVENDER 0xC325FFFF
#define GREEN 0x08FB0CFF
#define DARK_GREEN 0x1C421FFF
#define YELLOW 0xF7FB08FF
#define GOLD 0xFFD100FF
#define ORANGE 0xFF8B00FF
#define SAGE 0x6FB97FFF
#define BLUE 0x087EFBFF
#define SKY_BLUE 0x28AEFFFF
#define DARK_BLUE 0x0C08FBFF
#define TURQUOISE 0x08F7FBFF
#define WHITE 0xFFFFFFFF

// Timing
#define DEFAULT_TARGET_CYCLES_PER_SECOND 700

#endif
