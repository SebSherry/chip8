#ifndef DEBUG_H_
#define DEBUG_H_

#define debug(d, f) ((d) == NULL ? 0 : f)

#include "structs.h"

void init_debugger(Debugger *debugger);
int debug_prompt_user(Debugger *debugger, Chip8 *chip);
void debug_instruction(Instruction *instruction);
void halt_if_breakpoint(Chip8 *chip, char *instruction);
#endif
