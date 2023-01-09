#ifndef CHIP8_H_
#define CHIP8_H_

#define FONTSET_SIZE 80
#define FONT_START_MEMORY_ADDR 0x50
#define ROM_START_MEMORY_ADDR 0x200 

#include <stdint.h>
#include "io.h"

typedef struct {
    int debug;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t pc;
    uint16_t iregister;
    uint8_t memory[4096];
    uint8_t registers[16];
    uint16_t stack[16];
    uint32_t screen[SCREEN_SIZE]; 
} Chip8;

typedef struct {
    uint8_t instruction;
    uint8_t x;
    uint8_t y;
    uint8_t n;
    uint8_t nn;
    uint16_t nnn;
} Instruction;  

void init_chip8(Chip8 *chip, int debug);
int load_rom(Chip8 *chip, char *rom_filename);
void cycle(Chip8 *chip);
// Fetch and Decode the next instruction
void decode(Chip8 *chip, Instruction *instruction);   

// Instruction Implementations 
void exec_00E0(Chip8 *chip);   
//void exec_00EE(Chip8 *chip);   
void exec_1NNN(Chip8 *chip, uint16_t nnn);   
//void exec_2NNN(Chip8 *chip, uint16_t nnn);   
// void exec_3XNN(Chip8 *chip, uint8_t x, uint8_t nn);   
// void exec_4XNN(Chip8 *chip, uint8_t x, uint8_t nn);   
// void exec_5XY0(Chip8 *chip, uint8_t x, uint8_t y);   
void exec_6XNN(Chip8 *chip, uint8_t x, uint8_t nn);   
void exec_7XNN(Chip8 *chip, uint8_t x, uint8_t nn);   
// void exec_9XY0(Chip8 *chip, uint8_t x, uint8_t y);   
void exec_ANNN(Chip8 *chip, uint16_t nnn);   
void exec_DXYN(Chip8 *chip, uint8_t x, uint8_t y, uint8_t n);   
#endif
