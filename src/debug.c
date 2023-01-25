#include "chip8.h"
#include "debug.h"
#include "structs.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void init_debugger(Debugger *debugger) {
    // By default we step through each instruction
    debugger->stepping = true;
    debugger->exit = false;

    // Disable any breakpoints
    memset(debugger->breakpoints, 0, sizeof(debugger->breakpoints));

    // Create the instuction map
    strcpy(debugger->instruction_map[0], "00E0\0");
    strcpy(debugger->instruction_map[1], "00EE\0");
    strcpy(debugger->instruction_map[2], "1NNN\0");
    strcpy(debugger->instruction_map[3], "2NNN\0");
    strcpy(debugger->instruction_map[4], "3XNN\0");
    strcpy(debugger->instruction_map[5], "4XNN\0");
    strcpy(debugger->instruction_map[6], "5XY0\0");
    strcpy(debugger->instruction_map[7], "6XNN\0");
    strcpy(debugger->instruction_map[8], "7XNN\0");
    strcpy(debugger->instruction_map[9], "8XY0\0");
    strcpy(debugger->instruction_map[10], "8XY1\0");
    strcpy(debugger->instruction_map[11], "8XY2\0");
    strcpy(debugger->instruction_map[12], "8XY3\0");
    strcpy(debugger->instruction_map[13], "8XY4\0");
    strcpy(debugger->instruction_map[14], "8XY5\0");
    strcpy(debugger->instruction_map[15], "8XY6\0");
    strcpy(debugger->instruction_map[16], "8XY7\0");
    strcpy(debugger->instruction_map[17], "8XYE\0");
    strcpy(debugger->instruction_map[18], "9XY0\0");
    strcpy(debugger->instruction_map[19], "ANNN\0");
    strcpy(debugger->instruction_map[20], "BNNN\0");
    strcpy(debugger->instruction_map[21], "CXNN\0");
    strcpy(debugger->instruction_map[22], "DXYN\0");
    strcpy(debugger->instruction_map[23], "EX9E\0");
    strcpy(debugger->instruction_map[24], "EXA1\0");
    strcpy(debugger->instruction_map[25], "FX07\0");
    strcpy(debugger->instruction_map[26], "FX15\0");
    strcpy(debugger->instruction_map[27], "FX18\0");
    strcpy(debugger->instruction_map[28], "FX1E\0");
    strcpy(debugger->instruction_map[29], "FX0A\0");
    strcpy(debugger->instruction_map[30], "FX29\0");
    strcpy(debugger->instruction_map[31], "FX33\0");
    strcpy(debugger->instruction_map[32], "FX55\0");
    strcpy(debugger->instruction_map[33], "FX65\0");
}

void cleanup_debugger(Debugger *debugger) {
    free(debugger);
}

int debug_prompt_user(Debugger *debugger, Chip8 *chip) {
    bool valid = false;
    int result = 0;
    printf("\nPaused Execution\n");
    while(!valid) {
        printf("Please enter one of the following:\ng: Show State | n: Step | b: Set Breakpoint on instruction | m: Continue till breakpoint | k: Quit\n");
        
        char c = getchar();
        while (getchar() != '\n') {}
        valid = true;
        
        switch (c) {
            case 'k':
                result = 1;
                break;
            case 'n':
                // Enable stepping
                debugger->stepping = true;
                break;
            case 'b':
                // We want to loop regardless
                valid = false;
                printf("1.  00E0 Clear    11. 8XY1 BIN OR   21. BNNN Jump OFF  31. FX29 Font Char \n");
                printf("2.  00EE Return   12. 8XY2 BIN AND  22. CXNN Random    32. FX33 Decimal\n");
                printf("3.  1NNN Jump     13. 8XY3 LOG XOR  23. DXYN Display   33. FX55 Store\n");
                printf("4.  2NNN Sub      14. 8XY4 ADD XY   24. EX9E Skip Key  34. FX65 Load\n");
                printf("5.  3XNN Skip=    15. 8XY5 SUB XY   25. EXA1 Skip !Key\n");
                printf("6.  4XNN Skip!=   16. 8XY6 RSHIFT   26. FX07 X = Delay\n");
                printf("7.  5XY0 Skipx=y  17. 8XY7 SUB YX   27. FX15 Delay\n");
                printf("8.  6XNN Set NN   18. 8XYE LSHIFT   28. FX18 Sound\n");
                printf("9.  7XNN Add NN   19. 9XY0 Skipx!=y 29. FX1E IDX + X\n");
                printf("10. 8XY0 Set x=y  20. ANNN Set IDX  30. FX0A Get Key\n");
                printf("Please select an instruction: ");
                
                int selection;
                scanf("%d", &selection);
                // get newline
                getchar();
                
                if (selection < 1 || selection > 34) {
                    printf("Invalid input\n");
                } else {
                    if (debugger->breakpoints[selection-1]) {
                        printf("Breakpoint is ready set for this instruction. Would you like to remove it? (y/n): ");
                        char response = getchar();
                        // Get char again to remove the newline
                        // TODO handle longer input
                        getchar();
                        if (response == 'y') {
                            printf("Removing breakpoint on %s\n", debugger->instruction_map[selection-1]);
                            debugger->breakpoints[selection-1] = false;
                        }
                        // Ignore invalid input/negative respone
                        
                    } else {
                        printf("Setting breakpoint on %s\n", debugger->instruction_map[selection-1]);
                        debugger->breakpoints[selection-1] = true;
                    }
                }
                break;
            case 'm':
                // Continue until breakpoint
                debugger->stepping = false;
                break;
            case 'g':
                // We want to loop again
                valid = false;
                printf("PC:          %#08x\n", chip->pc);
                printf("IRegister:   %#08x\n", chip->iregister);
                printf("Delay Timer: %d\n", chip->delay_timer);
                printf("Sound Timer: %d\n", chip->delay_timer);
                
                // Registers
                for (int r = 0; r < NUM_OF_REGISTERS; r++) {
                    printf("V%X:          %X (%d)\n", r, chip->registers[r], chip->registers[r]);
                }

                // Key state
                printf("Keys Pressed: ");
                for (int i = 0; i < 16; i++) {
                    printf(" %d", chip->keys_pressed[i]);
                }
                printf("\nKeys Snapshot:");
                for (int i = 0; i < 16; i++) {
                    printf(" %d", chip->keys_snapshot[i]);
                }
                
                break;
            default:
                printf("Invalid input\n");
                valid = false;
                break;
        }
    }
    return result;
}

void debug_instruction(Instruction *instruction) {
    printf("Current instruction: %X%X%X%X\n", instruction->instruction, instruction->x, instruction->y, instruction->n);
    printf("NN: %X, NNN:%X\n", instruction->nn, instruction->nnn);
}

void halt_if_breakpoint(Chip8 *chip, char *instruction) {
    printf("Executing %s\n", instruction);
    
    int index = -1;
    for (int i = 0; i < NUM_OF_INSTRUCTIONS; i++) {
        if (strcmp(instruction, chip->debugger->instruction_map[i]) == 0) {
            index = i;
            break;
        }
    }
    
    if (index > -1 && chip->debugger->breakpoints[index]) {
        // No \n here because debug_prompt_user handles this for us
        // Not a great practice but it works
        printf("Hit breakpoint");
        chip->debugger->exit = debug_prompt_user(chip->debugger, chip);
    }
}