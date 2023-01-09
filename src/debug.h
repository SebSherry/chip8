#ifndef DEBUG_H_
#define DEBUG_H_

#define debug_printf(d, ...) ((d) == 0 ? 0 : printf(__VA_ARGS__))

#include "chip8.h"

int debug_prompt_user();
void debug_instruction(int debug, Instruction *instruction);

#endif
