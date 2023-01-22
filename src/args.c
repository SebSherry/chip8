#include "args.h"
#include "consts.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void usage() { 
    printf("Usage: ./chip8 [OPTIONS] <rom file>\n"); 
    printf("\nOptions:\n");
    printf("    -d, --debug            Run the debugger\n");
    printf("    -s, --scale [SCALE]    Set window scale (Default 10, minimum 1)\n");
    printf("    -f [COLOUR]            Set Foreground colour (See Below)\n");
    printf("    -b [COLOUR]            Set Background colour (See Below)\n");
    printf("    -c, --cycles [CYCLES]  Target CPU cycles per second\n");


    printf("\nAvailable Colours:\n");
    printf("    1 Black\n");
    printf("    2 Red\n");
    printf("    3 Dark Red\n");
    printf("    4 Magenta\n");
    printf("    5 Lavender\n");
    printf("    6 Green\n");
    printf("    7 Dark Green\n");
    printf("    8 Yellow\n");
    printf("    9 Gold\n");
    printf("    10 Orange\n");
    printf("    11 Sage\n");
    printf("    12 Blue\n");
    printf("    13 Sky Blue\n");
    printf("    14 Dark Blue\n");
    printf("    15 Turqiouse\n");
    printf("    16 White\n");
}

int read_args(Args *args, int argc, char *argv[]) {
    // Set the defaults 
    args->rom = NULL;
    args->debug = false;
    args->help = false;
    args->scale = DEFAULT_SCALE;
    args->foreground = WHITE;
    args->background = BLACK;
    args->target_cycles = DEFAULT_TARGET_CYCLES_PER_SECOND;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0) {
                args->debug = 1;
            } else if (strcmp(argv[i], "--scale") == 0 || strcmp(argv[i], "-s") == 0) {
                i++;
                if (i == argc) {
                    printf("ERROR: Scale parameter not provided\n");
                    return 1;
                }
                
                long scale = strtol(argv[i], NULL, 10);
                if (scale < MINIMUM_SCALE) {
                    printf("ERROR: Scale is less than %d\n", MINIMUM_SCALE);
                    return 1;
                }
                args->scale = (uint32_t) scale;
            } else if (strcmp(argv[i], "--cycles") == 0 || strcmp(argv[i], "-c") == 0) {
                i++;
                if (i == argc) {
                    printf("ERROR: Cycles parameter not provided\n");
                    return 1;
                }
                
                long cycles = strtol(argv[i], NULL, 10);
                if (cycles <= 0) {
                    printf("ERROR: Cycles must be a non zero positive number\n");
                    return 1;
                }
                args->target_cycles = (uint32_t) cycles;
            } else if (strcmp(argv[i], "-f") == 0) {
                i++;
                if (i == argc) {
                    printf("ERROR: Foreground colour not provided\n");
                    return 1;
                }

                uint32_t colour;
                if (!convert_input_to_colour(strtol(argv[i], NULL, 10), &colour)) {
                    printf("ERROR: Invalid foreground colour\n");
                    return 1;
                }
                
                args->foreground = (uint32_t) colour;
            } else if (strcmp(argv[i], "-b") == 0) {
                i++;
                if (i == argc) {
                    printf("ERROR: Background colour not provided\n");
                    return 1;
                }

                uint32_t colour;
                if (!convert_input_to_colour(strtol(argv[i], NULL, 10), &colour)) {
                    printf("ERROR: Invalid Background colour\n");
                    return 1;
                }
                
                args->background = (uint32_t) colour;
            } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
                args->help = true;
            } else {
                printf("Invalid option: %s\n", argv[i]);
                return 1;
            }
        } else {
            args->rom = argv[i];
            // We do not expect any more arguments after the rom
            // So we should stop processing them
            break;
        }
    }

    return 0;
}

bool convert_input_to_colour(long input, uint32_t *colour) {
    bool success = true;
    
    switch (input) {
        case 1:
            *colour = BLACK;
            break;
        case 2:
            *colour = RED;
            break;
        case 3:
            *colour = DARK_RED;
            break;
        case 4:
            *colour = MAGENTA;
            break;
        case 5:
            *colour = LAVENDER;
            break;
        case 6:
            *colour = GREEN;
            break;
        case 7:
            *colour = DARK_GREEN;
            break;
        case 8:
            *colour = YELLOW;
            break;
        case 9:
            *colour = GOLD;
            break;
        case 10:
            *colour = ORANGE;
            break;
        case 11:
            *colour = SAGE;
            break;
        case 12:
            *colour = BLUE;
            break;
        case 13:
            *colour = SKY_BLUE;
            break;
        case 14:
            *colour = DARK_BLUE;
            break;
        case 15:
            *colour = TURQUOISE;
            break;
        case 16:
            *colour = WHITE;
            break;
        default:
            success = false;
    }

    return success;
}
