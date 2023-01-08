#include "chip8.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void init_chip8(Chip8 *chip) {
    chip->delay_timer = 0;
    chip->sound_timer = 0;
    chip->pc = 0;
    chip->iregister = 0;

    memset(chip->memory, 0, sizeof(chip->memory));
    memset(chip->registers, 0, sizeof(chip->registers));
    memset(chip->screen, 0, sizeof(chip->screen));
    memset(chip->stack, 0, sizeof(chip->stack));

    // Load font into memory
    uint8_t fontset[FONTSET_SIZE] = { 
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0 
        0x20, 0x60, 0x20, 0x20, 0x70, // 1 
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2 
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3 
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4 
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5 
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6 
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7 
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8 
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A 
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B 
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C 
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D 
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F 
    };

    for (int i = 0; i < FONTSET_SIZE; ++i) { 
        chip->memory[FONT_START_MEMORY_ADDR + i] = fontset[i]; 
    } 
}

int load_rom(Chip8 *chip, char *rom_filename) {
    uint8_t buffer[100] = { 0 };
    int memory_ptr = ROM_START_MEMORY_ADDR;
    FILE *fp = NULL;
    int result = 0;

    fp = fopen(rom_filename, "r");
    if (fp == NULL) {
        printf("Failed to open rom file %s\n", rom_filename);
        return 1;
    }

    while(!feof(fp)) {
        unsigned long read = fread(&buffer, sizeof(chip->memory[0]), 100, fp);  

        for (int i = 0; i < read; i++) {
            chip->memory[memory_ptr] = buffer[i];
            memory_ptr++;

            if (memory_ptr == 4096) {
                printf("Error reading rom file %s. Out of memory\n", rom_filename);
                result = 1;
                break; 
            }
        }

        if (result != 0) break; 
    } 

    fclose(fp);

    // If successful set the PC to the start of the ROM
    // We could argue this should be set in init_chip8 since it's the same everytime
    // Setting it here because the PC shouldn't point to anything if the rom wasn't loaded
    if (!result) {
        chip->pc = ROM_START_MEMORY_ADDR;
    }
    
    return result;
}

void cycle(Chip8 *chip, int screen_pointer, int colour) {
    chip->screen[screen_pointer] = colour;
}
