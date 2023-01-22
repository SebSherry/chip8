#ifndef CHIP8_H_
#define CHIP8_H_

#include "structs.h"
#include "consts.h"
#include <stdint.h>
#include <stdbool.h>

void init_chip8(Chip8 *chip, Debugger *debug, uint32_t foreground, uint32_t background);
void cleanup_chip8(Chip8 *chip);
int load_rom(Chip8 *chip, char *rom_filename);
bool cycle(Chip8 *chip);
// Fetch and Decode the next instruction
void decode(Chip8 *chip, Instruction *instruction);   
void update_timers(Chip8 *chip);

// Instruction Implementations 
void exec_00E0(Chip8 *chip);   
void exec_00EE(Chip8 *chip);   
void exec_1NNN(Chip8 *chip, uint16_t nnn);   
void exec_2NNN(Chip8 *chip, uint16_t nnn);   
void exec_3XNN(Chip8 *chip, uint8_t x, uint8_t nn);   
void exec_4XNN(Chip8 *chip, uint8_t x, uint8_t nn);   
void exec_5XY0(Chip8 *chip, uint8_t x, uint8_t y);   
void exec_6XNN(Chip8 *chip, uint8_t x, uint8_t nn);   
void exec_7XNN(Chip8 *chip, uint8_t x, uint8_t nn);   
void exec_8XY0(Chip8 *chip, uint8_t x, uint8_t y);
void exec_8XY1(Chip8 *chip, uint8_t x, uint8_t y);
void exec_8XY2(Chip8 *chip, uint8_t x, uint8_t y);
void exec_8XY3(Chip8 *chip, uint8_t x, uint8_t y);
void exec_8XY4(Chip8 *chip, uint8_t x, uint8_t y);
void exec_8XY5(Chip8 *chip, uint8_t x, uint8_t y);
void exec_8XY6(Chip8 *chip, uint8_t x, uint8_t y);
void exec_8XY7(Chip8 *chip, uint8_t x, uint8_t y);
void exec_8XYE(Chip8 *chip, uint8_t x, uint8_t y);
void exec_9XY0(Chip8 *chip, uint8_t x, uint8_t y);   
void exec_ANNN(Chip8 *chip, uint16_t nnn); 
void exec_BNNN(Chip8 *chip, uint16_t nnn); 
void exec_CXNN(Chip8 *chip, uint8_t x, uint8_t nn); 
void exec_DXYN(Chip8 *chip, uint8_t x, uint8_t y, uint8_t n);   
void exec_EX9E(Chip8 *chip, uint8_t x);
void exec_EXA1(Chip8 *chip, uint8_t x);
void exec_FX07(Chip8 *chip, uint8_t x);
void exec_FX15(Chip8 *chip, uint8_t x);
void exec_FX18(Chip8 *chip, uint8_t x);
void exec_FX1E(Chip8 *chip, uint8_t x);
void exec_FX0A(Chip8 *chip, uint8_t x);
void exec_FX29(Chip8 *chip, uint8_t x);
void exec_FX33(Chip8 *chip, uint8_t x);
void exec_FX55(Chip8 *chip, uint8_t x);
void exec_FX65(Chip8 *chip, uint8_t x);
#endif
