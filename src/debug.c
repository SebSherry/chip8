#include "chip8.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

int debug_prompt_user() {
	bool valid = false;
	int result = 0;
	
	printf("\nPaused Execution\n");

	while(!valid) {
		printf("Enter 'n' to continue, 'q' to quit: \n");
	
		char c = getchar();
		while (getchar() != '\n') {} 

		if (c == 'q' || c == 'n') {
			valid = true;
			if (c == 'q') {
				result = 1;
			}
		} else {
			printf("Invalid input\n");	
		}
	}
	
	return result;
}

void debug_instruction(int debug, Instruction *instruction) {
	if (debug) {
		printf("Current instruction: %X%X%X%X\n", instruction->instruction, instruction->x, instruction->y, instruction->n);
		printf("NN: %X, NNN:%X\n", instruction->nn, instruction->nnn);
	}
}
