#ifndef CHIP8_H_
#define CHIP8_H_

#define FONTSET_SIZE 80
#define FONT_START_MEMORY_ADDR 0x50
#define ROM_START_MEMORY_ADDR 0x200 


#include <stdint.h>
#include "io.h"

typedef struct {
   uint8_t delay_timer;
   uint8_t sound_timer;
   uint16_t pc;
   uint16_t iregister;
   uint8_t memory[4096];
   uint8_t registers[16];
   uint16_t stack[16];
   uint32_t screen[SCREEN_SIZE]; 
} Chip8;

void init_chip8(Chip8 *chip);
int load_rom(Chip8 *chip, char *rom_filename);
void cycle(Chip8 *chip, int screen_pointer, int colour);

#endif
