#include "chip8.h"
#include "io.h"
#include "debug.h"
#include "structs.h"
#include "consts.h"
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void init_chip8(Chip8 *chip, Debugger *debug, uint32_t foreground, uint32_t background) {
    chip->delay_timer = 0;
    chip->sound_timer = 0;
    chip->pc = 0;
    chip->iregister = 0;
    chip->stack_pointer = 0;
    chip->debugger = debug;
    chip->foreground_colour = foreground;
    chip->background_colour = background; 
    chip->waiting_to_draw = 0;
    chip->display_interrupt_triggered = false;
    chip->keys_pressed = 0;
    chip->keys_snapshot = 0;
    
    memset(chip->memory, 0, sizeof(chip->memory));
    memset(chip->registers, 0, sizeof(chip->registers));
    memset(chip->stack, 0, sizeof(chip->stack));

    // Call 00E0 to keep the clear screen behaviour consistent
    debug(chip->debugger, printf("Initializing Screen with 00E0\n"));
    exec_00E0(chip);
    
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

    // Seed random generator for CXNN instruction
    srand(time(NULL));
}

void cleanup_chip8(Chip8 *chip) {
    if (chip->debugger != NULL) {
        cleanup_debugger(chip->debugger);
        chip->debugger = NULL;
    }
    
    chip = NULL;
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
    
    debug(chip->debugger, debug_instruction(&instruction));
    
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
            chip->waiting_to_draw++;
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

void update_timers(Chip8 *chip) {
    if (chip->delay_timer > 0) {
        debug(chip->debugger, printf("Decrement Delay Timer\n"));
        chip->delay_timer--;
    }

    if (chip->sound_timer > 0) {
        debug(chip->debugger, printf("Decrement Sound Timer\n"));
        chip->sound_timer--;
    }
}

// Instruction Implementations 
void exec_00E0(Chip8 *chip) {
    debug(chip->debugger, halt_if_breakpoint(chip, "00E0"));
    // Since the colours are configurable we can't use memset
    for (int i = 0; i < SCREEN_SIZE; i++) {
        chip->screen[i] = chip->background_colour;
    }
}
  
void exec_00EE(Chip8 *chip) {
    debug(chip->debugger, halt_if_breakpoint(chip, "00EE"));

    // TODO Check for stack underflow
    chip->stack_pointer--;
    chip->pc = chip->stack[chip->stack_pointer];
    chip->stack[chip->stack_pointer] = 0;   
}
   
void exec_1NNN(Chip8 *chip, uint16_t nnn) {
    debug(chip->debugger, halt_if_breakpoint(chip, "1NNN"));
    chip->pc = nnn;
}
   
void exec_2NNN(Chip8 *chip, uint16_t nnn) {
    debug(chip->debugger, halt_if_breakpoint(chip, "2NNN"));

    // TODO Check for stack overflow
    chip->stack[chip->stack_pointer] = chip->pc;
    chip->stack_pointer++;
    
    chip->pc = nnn;
}
   
void exec_3XNN(Chip8 *chip, uint8_t x, uint8_t nn) {
    debug(chip->debugger, halt_if_breakpoint(chip, "3XNN"));
    
    if (chip->registers[x] == nn) {
        chip->pc += 2;   
    }
}
   
void exec_4XNN(Chip8 *chip, uint8_t x, uint8_t nn) {
    debug(chip->debugger, halt_if_breakpoint(chip, "4XNN"));

    if (chip->registers[x] != nn) {
        chip->pc += 2;   
    }
}
   
void exec_5XY0(Chip8 *chip, uint8_t x, uint8_t y) {
    debug(chip->debugger, halt_if_breakpoint(chip, "5XY0"));

    if (chip->registers[x] == chip->registers[y]) {
        chip->pc += 2;   
    }
}
   
void exec_6XNN(Chip8 *chip, uint8_t x, uint8_t nn) {
    debug(chip->debugger, halt_if_breakpoint(chip, "6XNN"));
    
    chip->registers[x] = nn;
}
   
void exec_7XNN(Chip8 *chip, uint8_t x, uint8_t nn) {
    debug(chip->debugger, halt_if_breakpoint(chip, "7XNN"));
    
    chip->registers[x] += nn;
}   
   
void exec_8XY0(Chip8 *chip, uint8_t x, uint8_t y) {
    debug(chip->debugger, halt_if_breakpoint(chip, "8XY0"));
    chip->registers[x] = chip->registers[y];
}

void exec_8XY1(Chip8 *chip, uint8_t x, uint8_t y) {
    debug(chip->debugger, halt_if_breakpoint(chip, "8XY1"));
    chip->registers[x] = chip->registers[x] | chip->registers[y];
    chip->registers[0xF] = 0;
}

void exec_8XY2(Chip8 *chip, uint8_t x, uint8_t y) {
    debug(chip->debugger, halt_if_breakpoint(chip, "8XY2"));
    chip->registers[x] = chip->registers[x] & chip->registers[y];
    chip->registers[0xF] = 0;
}

void exec_8XY3(Chip8 *chip, uint8_t x, uint8_t y) {
    debug(chip->debugger, halt_if_breakpoint(chip, "8XY3"));
    chip->registers[x] = chip->registers[x] ^ chip->registers[y];
    chip->registers[0xF] = 0;
}

void exec_8XY4(Chip8 *chip, uint8_t x, uint8_t y) {
    debug(chip->debugger, halt_if_breakpoint(chip, "8XY4"));
    uint8_t vx = chip->registers[x];
    uint8_t vy = chip->registers[y];
    
    chip->registers[x] = vx + vy;
    chip->registers[0xF] = vy > UINT8_MAX - vx ? 1 : 0;
}

void exec_8XY5(Chip8 *chip, uint8_t x, uint8_t y) {
    debug(chip->debugger, halt_if_breakpoint(chip, "8XY5"));
    uint8_t vx = chip->registers[x];
    uint8_t vy = chip->registers[y];
    chip->registers[x] = vx - vy;

    // Set VF to 1, then check for underflow
    if (vx > vy) {
        chip->registers[0xF] = 1;
    }

    if (vy > vx && vy < UINT8_MAX - vx) {
        chip->registers[0xF] = 0;
    }
}

void exec_8XY6(Chip8 *chip, uint8_t x, uint8_t y) {
    debug(chip->debugger, halt_if_breakpoint(chip, "8XY6"));
    
    // Set VX to VY?
    chip->registers[x] = chip->registers[y];

    // Shift 1 bit to the right
    uint8_t original_vx = chip->registers[x];
    chip->registers[x] = original_vx>>1;

    // Set VF to the result of the shifted bit
    chip->registers[0xF] = (original_vx & (1<<0));
}

void exec_8XY7(Chip8 *chip, uint8_t x, uint8_t y) {
    debug(chip->debugger, halt_if_breakpoint(chip, "8XY7"));
    uint8_t vx = chip->registers[x];
    uint8_t vy = chip->registers[y];
    chip->registers[x] = vy - vx;

    // Set VF to 1, then check for underflow
    if (vy > vx) {
        chip->registers[0xF] = 1;
    }

    if (vx > vy && vx < UINT8_MAX - vy) {
        chip->registers[0xF] = 0;
    }
}

void exec_8XYE(Chip8 *chip, uint8_t x, uint8_t y) {
    debug(chip->debugger, halt_if_breakpoint(chip, "8XYE"));
    // Set VX to VY?
    chip->registers[x] = chip->registers[y];

    // Shift 1 bit to the left
    uint8_t original_vx = chip->registers[x];
    chip->registers[x] = original_vx<<1;

    // Set VF to the result of the shifted bit
    chip->registers[0xF] = (original_vx & (1<<7)) > 0;
}

void exec_9XY0(Chip8 *chip, uint8_t x, uint8_t y) {
    debug(chip->debugger, halt_if_breakpoint(chip, "9XY0"));
    
    if (chip->registers[x] != chip->registers[y]) {
        chip->pc += 2;   
    }
}
   
void exec_ANNN(Chip8 *chip, uint16_t nnn) {
    debug(chip->debugger, halt_if_breakpoint(chip, "ANNN"));
    chip->iregister = nnn;
    debug(chip->debugger, printf("IRegister: %X\n", chip->iregister));
}
 
void exec_BNNN(Chip8 *chip, uint16_t nnn) {
    debug(chip->debugger, halt_if_breakpoint(chip, "BNNN"));
    chip->pc = nnn + chip->registers[0];
}
 
void exec_CXNN(Chip8 *chip, uint8_t x, uint8_t nn) {
    debug(chip->debugger, halt_if_breakpoint(chip, "CXNN"));
    chip->registers[x] = rand() & nn;
}
 
void exec_DXYN(Chip8 *chip, uint8_t x, uint8_t y, uint8_t n) {
    debug(chip->debugger, halt_if_breakpoint(chip, "DXYN"));

    // Halt Drawing until we hit the interrupt
    // This is pass the Display Quirk test in Timendus' test suite
    if (!chip->display_interrupt_triggered) {
        chip->pc -= 2;
        return;
    } 
        
    chip->display_interrupt_triggered = false; 
    
    // Get the X/Y coords to draw the sprite 
    int x_coord = chip->registers[x] % SCREEN_WIDTH;
    int y_coord = chip->registers[y] % SCREEN_HEIGHT;

    debug(chip->debugger, printf("x: %d, y: %d\n", x_coord, y_coord));

    // Premptively set VF to 0
    chip->registers[0xF] = 0;

    for (int i = 0; i < n; i++) {
        // Stop writing if we've hit the bottom
        if (y_coord == SCREEN_HEIGHT) break; 
        
        uint8_t sprite = chip->memory[chip->iregister + i];
        debug(chip->debugger, printf("Sprite at address %X: %X\n", chip->iregister + i, sprite));
        int screen_idx = (y_coord * SCREEN_WIDTH) + x_coord;
        debug(chip->debugger, printf("Screen Index: %d\n", screen_idx));

        for (int j = 0; j < 8; j++) {
            if (x_coord + j == SCREEN_WIDTH) { debug(chip->debugger, printf("Hit screen edge\n")); break; } 

            if ((sprite & 128) == 128) {
                if (chip->screen[screen_idx] == chip->foreground_colour) {
                    chip->screen[screen_idx] = chip->background_colour;
                    chip->registers[0xF] = 1;
                } else {
                    chip->screen[screen_idx] = chip->foreground_colour;
                }
            }
            
            screen_idx++;
            sprite = sprite<<1;
        }
        y_coord++;              
    }
}
   
void exec_EX9E(Chip8 *chip, uint8_t x) {
    debug(chip->debugger, halt_if_breakpoint(chip, "EX9E"));

    // TODO Bounds checking?
    uint16_t key = (chip->keys_pressed>>chip->registers[x]) & 1;
    
    if (key) {
        chip->pc += 2;   
    }
}

void exec_EXA1(Chip8 *chip, uint8_t x) {
    debug(chip->debugger, halt_if_breakpoint(chip, "EXA1"));
    
    // TODO Bounds checking?
    uint16_t key = (chip->keys_pressed>>chip->registers[x]) & 1;
    
    if (!key) {
        chip->pc += 2;   
    }
}

void exec_FX07(Chip8 *chip, uint8_t x) {
    debug(chip->debugger, halt_if_breakpoint(chip, "FX07"));
    chip->registers[x] = chip->delay_timer;
}

void exec_FX15(Chip8 *chip, uint8_t x) {
    debug(chip->debugger, halt_if_breakpoint(chip, "FX15"));
    chip->delay_timer = chip->registers[x];
}

void exec_FX18(Chip8 *chip, uint8_t x) {
    debug(chip->debugger, halt_if_breakpoint(chip, "FX18"));
    chip->sound_timer = chip->registers[x];
}

void exec_FX1E(Chip8 *chip, uint8_t x) {
    debug(chip->debugger, halt_if_breakpoint(chip, "FX1E"));

    uint16_t original_i = chip->iregister;
    chip->iregister += chip->registers[x];

    // Record an overflow in VF if the I register overflows above 1000
    if (chip->iregister > 1000 && chip->iregister < original_i) {
        chip->registers[0xF] = 1;
    }
}

void exec_FX0A(Chip8 *chip, uint8_t x) {
    debug(chip->debugger, halt_if_breakpoint(chip, "FX0A"));

    int pressed = -1;
    if (chip->keys_pressed != chip->keys_snapshot) {
        uint16_t diff = chip->keys_snapshot;
        uint16_t current = chip->keys_pressed;

        for (int i = 0; i < 16; i++) {
            if (((current & 1) == 0) && ((diff & 1) == 1)) {
                pressed = i;
                break;
            }
            diff >>= 1;
            current >>= 1;
        }

        chip->keys_snapshot = chip->keys_pressed;
    }


    if (pressed > -1) {
        chip->registers[x] = pressed;
        // Set the snapshot to 0 to avoid weird state
        chip->keys_snapshot = 0;
    } else {
        // Decrement the PC to halt execution
        chip->pc -= 2;
    }
}

void exec_FX29(Chip8 *chip, uint8_t x) {
    debug(chip->debugger, halt_if_breakpoint(chip, "FX29"));

    // We only want the first nibble
    uint8_t font_char = x>>4;

    chip->iregister = FONT_START_MEMORY_ADDR + (font_char * 5);
}

void exec_FX33(Chip8 *chip, uint8_t x) {
    debug(chip->debugger, halt_if_breakpoint(chip, "FX33"));

    uint8_t ones, tens, hundreds = 0;
    uint8_t vx = chip->registers[x];

    ones= vx % 10;
    tens = ((vx - ones) % 100) / 10;
    hundreds = (vx - tens - ones) / 100;
    
    uint16_t iregister = chip->iregister;
    chip->memory[iregister] = hundreds;
    chip->memory[iregister+1] = tens;
    chip->memory[iregister+2] = ones;
}

void exec_FX55(Chip8 *chip, uint8_t x) {
    debug(chip->debugger, halt_if_breakpoint(chip, "FX55"));

    // <= as this opperation is inclusive
    for (int j = 0; j <= x; j++) {
        chip->memory[chip->iregister] = chip->registers[j];   
        chip->iregister++;
    }
}

void exec_FX65(Chip8 *chip, uint8_t x) {
    debug(chip->debugger, halt_if_breakpoint(chip, "FX65"));
    // <= as this opperation is inclusive
    for (int j = 0; j <= x; j++) {
        chip->registers[j] = chip->memory[chip->iregister];   
        chip->iregister++;
    }
}