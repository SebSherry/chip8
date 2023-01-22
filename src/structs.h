#ifndef STRUCTS_H_
#define STRUCTS_H_

#include "consts.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    bool stepping;
    bool exit; 
    char instruction_map[NUM_OF_INSTRUCTIONS][5];
    bool breakpoints[NUM_OF_INSTRUCTIONS];
} Debugger;

typedef struct {
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t pc;
    uint16_t iregister;
    uint8_t memory[4096];
    uint8_t registers[NUM_OF_REGISTERS];
    uint8_t stack_pointer;
    uint16_t stack[16];
    uint32_t screen[SCREEN_SIZE];
    bool keys_pressed[16];
    uint32_t foreground_colour;
    uint32_t background_colour;

    // Debugger, is null if debugging disabled
    Debugger *debugger;
} Chip8;

typedef struct {
    uint8_t instruction;
    uint8_t x;
    uint8_t y;
    uint8_t n;
    uint8_t nn;
    uint16_t nnn;
} Instruction;  

#endif
