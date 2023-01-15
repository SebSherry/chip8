#include "chip8.h"
#include "io.h"
#include "debug.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void init_chip8(Chip8 *chip, int debug) {
    chip->delay_timer = 0;
    chip->sound_timer = 0;
    chip->pc = 0;
    chip->iregister = 0;
    chip->debug = debug;

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

void cycle(Chip8 *chip) {
    Instruction instruction = { 0 };

    // Fetch and Decode the next instruction
    decode(chip, &instruction);
    
    debug_instruction(chip->debug, &instruction);

    switch (instruction.instruction) {
        case 0x0:
            switch (instruction.nnn) {
                //00E0 Clear Screen    
                case 0x00E0:
                    exec_00E0(chip);
                    break;
                //00EE Return from Subroutine 
                case 0x00EE:
                    exec_00EE(chip);
                    break;
                // 0NNN Execute Machine Routine. Skipped
                default:
                    printf("Skipping 0%X%X%X instruction\n", instruction.x, instruction.y, instruction.n);
                    break;
            }
            break;
        case 0x1:
            // 1NNN Jump
            exec_1NNN(chip, instruction.nnn);
            break;     
        case 0x2:
            // 2NNN Execute Subroutine 
            exec_2NNN(chip, instruction.nnn);
            break;     
        case 0x3:
            // 3XNN Skip if VX = NN 
            exec_3XNN(chip, instruction.x, instruction.nn);
            break;     
        case 0x4:
            // 4XNN Skip if VX != NN 
            exec_4XNN(chip, instruction.x, instruction.nn);
            break;     
        case 0x5:
            if (instruction.n == 0) {
                // 5XY0 Skip if VX = VY 
                exec_5XY0(chip, instruction.x, instruction.y);
            } else {
                printf("UNDEFINED INSTRUCTION %X%X%X%X\n", instruction.instruction, instruction.x, instruction.y, instruction.n);
            }
            break;     
        case 0x6:
            // 6XNN Set Register VX            
            exec_6XNN(chip, instruction.x, instruction.nn);
            break;
        case 0x7:
            // 7XNN Add value to register VX
            exec_7XNN(chip, instruction.x, instruction.nn);
            break;
        case 0x8:
            switch (instruction.n) {
                case 0x0:
                    // 8XY0 Set
                    exec_8XY0(chip, instruction.x, instruction.y);
                    break;
                case 0x1:
                    // 8XY1 Binary Or
                    exec_8XY1(chip, instruction.x, instruction.y);
                    break;
                case 0x2:
                    // 8XY2 Binary And
                    exec_8XY2(chip, instruction.x, instruction.y);
                    break;
                case 0x3:
                    // 8XY3 Binary XOR
                    exec_8XY3(chip, instruction.x, instruction.y);
                    break;
                case 0x4:
                    // 8XY4 And
                    exec_8XY4(chip, instruction.x, instruction.y);
                    break;
                case 0x5:
                    // 8XY5 Subract (VX - VY)
                    exec_8XY5(chip, instruction.x, instruction.y);
                    break;
                case 0x6:
                    // 8XY6 Shift Right
                    exec_8XY6(chip, instruction.x, instruction.y);
                    break;
                case 0x7:
                    // 8XY7 Subtract (VY - VX)
                    exec_8XY7(chip, instruction.x, instruction.y);
                    break;
                case 0xE:
                    // 8XYE Shift Left
                    exec_8XYE(chip, instruction.x, instruction.y);
                    break;
                default:
                    printf("UNDEFINED INSTRUCTION %X%X%X%X\n", instruction.instruction, instruction.x, instruction.y, instruction.n);
                    break;
            }
            break;
        case 0x9:
            if (instruction.n == 0) {
                // 9XY0 Skip if VX != VY 
                exec_9XY0(chip, instruction.x, instruction.y);
            } else {
                printf("UNDEFINED INSTRUCTION %X%X%X%X\n", instruction.instruction, instruction.x, instruction.y, instruction.n);
            }
            break;     
        case 0xA:
            // ANNN Set Index register I
            exec_ANNN(chip, instruction.nnn);     
            break; 
        case 0xB:
            // BNNN Jump with offset
            exec_BNNN(chip, instruction.nnn);     
            break; 
        case 0xC:
            // CXNN Random
            exec_CXNN(chip, instruction.x, instruction.nn);     
            break; 
        case 0xD:
            // DXYN Display 
            exec_DXYN(chip, instruction.x, instruction.y, instruction.n);
            break;
        case 0xE:
            if (instruction.nn == 0x9E) {
                // EX9E Skip if pressed 
                exec_EX9E(chip, instruction.x);
            } else if (instruction.nn == 0xA1) {
                // EXA1 Skip if not pressed 
                exec_EXA1(chip, instruction.x);
            } else {
                printf("UNDEFINED INSTRUCTION %X%X%X%X\n", instruction.instruction, instruction.x, instruction.y, instruction.n);
            }            
            break;
        case 0xF:
            if (instruction.x != 0) {
                printf("UNDEFINED INSTRUCTION %X%X%X%X\n", instruction.instruction, instruction.x, instruction.y, instruction.n);
            } else {
                switch (instruction.nn) {
                    case 0x07:
                        // FX07 Read Delay Timer
                        exec_FX07(chip, instruction.x);
                        break;
                    case 0x15:
                        // FX15 Set Delay Timer
                        exec_FX15(chip, instruction.x);
                        break;
                    case 0x18:
                        // FX18 Set Sound Timer
                        exec_FX18(chip, instruction.x);
                        break;
                    case 0x1E:
                        // FX1E Add to Index
                        exec_FX1E(chip, instruction.x);
                        break;
                    case 0x0A:
                        // FX0A Get Key
                        exec_FX0A(chip, instruction.x);
                        break;
                    case 0x29:
                        // FX29 Font Character
                        exec_FX29(chip, instruction.x);
                        break;
                    case 0x33:
                        // FX33 Binary-coded decimal conversion
                        exec_FX33(chip, instruction.x);
                        break;
                    case 0x55:
                        // FX55 Store Memory
                        exec_FX55(chip, instruction.x);
                        break;
                    case 0x65:
                        // FX65 Load Memory
                        exec_FX65(chip, instruction.x);
                        break;
                    default:
                        printf("UNDEFINED INSTRUCTION %X%X%X%X\n", instruction.instruction, instruction.x, instruction.y, instruction.n);
                        break;
                }
            }
            break;
        default:
            printf("UNDEFINED INSTRUCTION %X%X%X%X\n", instruction.instruction, instruction.x, instruction.y, instruction.n);
            break;
    } 
}

void decode(Chip8 *chip, Instruction *instruction) {
    instruction->instruction = (chip->memory[chip->pc] & 240)>>4;
    instruction->x = chip->memory[chip->pc] & 15;
    instruction->nnn = ((uint16_t) instruction->x)<<8;

    instruction->nn = chip->memory[chip->pc + 1];
    instruction->y = (instruction->nn & 240)>>4;
    instruction->n = instruction->nn & 15;
    instruction->nnn |= (uint16_t) instruction->nn;

    chip->pc += 2;
}

   
// Instruction Implementations 
void exec_00E0(Chip8 *chip) {
    debug_printf(chip->debug, "Executing 00E0\n");
    memset(chip->screen, 0, sizeof(chip->screen)); 
}
  
void exec_00EE(Chip8 *chip) {
    debug_printf(chip->debug, "Executing 00EE\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}
   
void exec_1NNN(Chip8 *chip, uint16_t nnn) {
    debug_printf(chip->debug, "Executing 1NNN\n");
    chip->pc = nnn;
}
   
void exec_2NNN(Chip8 *chip, uint16_t nnn) {
    debug_printf(chip->debug, "Executing 2NNN\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}
   
void exec_3XNN(Chip8 *chip, uint8_t x, uint8_t nn) {
    debug_printf(chip->debug, "Executing 3XNN\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}
   
void exec_4XNN(Chip8 *chip, uint8_t x, uint8_t nn) {
    debug_printf(chip->debug, "Executing 4XNN\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}
   
void exec_5XY0(Chip8 *chip, uint8_t x, uint8_t y) {
    debug_printf(chip->debug, "Executing 5XY0\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}
   
void exec_6XNN(Chip8 *chip, uint8_t x, uint8_t nn) {
    debug_printf(chip->debug, "Executing 6XNN\n");
    
    chip->registers[x] = nn;
    
    debug_printf(chip->debug, "V%X: %X (%d)\n", x, chip->registers[x], chip->registers[x]);
}
   
void exec_7XNN(Chip8 *chip, uint8_t x, uint8_t nn) {
    debug_printf(chip->debug, "Executing 7XNN\n");
    
    chip->registers[x] += nn;

    debug_printf(chip->debug, "V%X: %X (%d)\n", x, chip->registers[x], chip->registers[x]);
}   
   
void exec_8XY0(Chip8 *chip, uint8_t x, uint8_t y) {
    debug_printf(chip->debug, "Executing 8XY0\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_8XY1(Chip8 *chip, uint8_t x, uint8_t y) {
    debug_printf(chip->debug, "Executing 8XY1\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_8XY2(Chip8 *chip, uint8_t x, uint8_t y) {
    debug_printf(chip->debug, "Executing 8XY2\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_8XY3(Chip8 *chip, uint8_t x, uint8_t y) {
    debug_printf(chip->debug, "Executing 8XY3\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_8XY4(Chip8 *chip, uint8_t x, uint8_t y) {
    debug_printf(chip->debug, "Executing 8XY4\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_8XY5(Chip8 *chip, uint8_t x, uint8_t y) {
    debug_printf(chip->debug, "Executing 8XY5\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_8XY6(Chip8 *chip, uint8_t x, uint8_t y) {
    debug_printf(chip->debug, "Executing 8XY6\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_8XY7(Chip8 *chip, uint8_t x, uint8_t y) {
    debug_printf(chip->debug, "Executing 8XY7\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_8XYE(Chip8 *chip, uint8_t x, uint8_t y) {
    debug_printf(chip->debug, "Executing 8XYE\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_9XY0(Chip8 *chip, uint8_t x, uint8_t y) {
    debug_printf(chip->debug, "Executing 9XY0\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}
   
void exec_ANNN(Chip8 *chip, uint16_t nnn) {
    debug_printf(chip->debug, "Executing ANNN\n");
    chip->iregister = nnn;
    debug_printf(chip->debug, "IRegister: %X\n", chip->iregister);
}
 
void exec_BNNN(Chip8 *chip, uint16_t nnn) {
    debug_printf(chip->debug, "Executing BNNN\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}
 
void exec_CXNN(Chip8 *chip, uint8_t x, uint8_t nn) {
    debug_printf(chip->debug, "Executing CXNN\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}
 
void exec_DXYN(Chip8 *chip, uint8_t x, uint8_t y, uint8_t n) {
    debug_printf(chip->debug, "Executing DXYN\n");
    // Get the X/Y coords to draw the sprite 
    int x_coord = chip->registers[x] % SCREEN_WIDTH;
    int y_coord = chip->registers[y] % SCREEN_HEIGHT;

    debug_printf(chip->debug, "x: %d, y: %d\n", x_coord, y_coord);

    // Premptively set VF to 0
    chip->registers[0xF] = 0;

    for (int i = 0; i < n; i++) {
        // Stop writing if we've hit the bottom
        if (y_coord == SCREEN_HEIGHT) break; 
        
        uint8_t sprite = chip->memory[chip->iregister + i];
        debug_printf(chip->debug, "Sprite at address %X: %X\n", chip->iregister + i, sprite);
        int screen_idx = (y_coord * SCREEN_WIDTH) + x_coord;
        debug_printf(chip->debug, "Screen Index: %d\n", screen_idx);

        for (int j = 0; j < 8; j++) {
            if (x_coord + j == SCREEN_WIDTH) { debug_printf(chip->debug, "Hit screen edge\n"); break; } 

            if ((sprite & 128) == 128) {
                if (chip->screen[screen_idx] == PIXEL_WHITE) {
                    chip->screen[screen_idx] = PIXEL_BLACK;
                    chip->registers[0xF] = 1;
                } else {
                    chip->screen[screen_idx] = PIXEL_WHITE;
                }
            }
            
            screen_idx++;
            sprite = sprite<<1;
        }
        y_coord++;              
    }
}
   
void exec_EX9E(Chip8 *chip, uint8_t x) {
    debug_printf(chip->debug, "Executing EX9E\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_EXA1(Chip8 *chip, uint8_t x) {
    debug_printf(chip->debug, "Executing EXA1\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_FX07(Chip8 *chip, uint8_t x) {
    debug_printf(chip->debug, "Executing FX07\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_FX15(Chip8 *chip, uint8_t x) {
    debug_printf(chip->debug, "Executing FX15\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_FX18(Chip8 *chip, uint8_t x) {
    debug_printf(chip->debug, "Executing FX18\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_FX1E(Chip8 *chip, uint8_t x) {
    debug_printf(chip->debug, "Executing FX1E\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_FX0A(Chip8 *chip, uint8_t x) {
    debug_printf(chip->debug, "Executing FX0A\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_FX29(Chip8 *chip, uint8_t x) {
    debug_printf(chip->debug, "Executing FX29\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_FX33(Chip8 *chip, uint8_t x) {
    debug_printf(chip->debug, "Executing FX33\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_FX55(Chip8 *chip, uint8_t x) {
    debug_printf(chip->debug, "Executing FX55\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}

void exec_FX65(Chip8 *chip, uint8_t x) {
    debug_printf(chip->debug, "Executing FX65\n");
    printf("INSTRUCTION NOT YET IMPLEMENTED\n");
}