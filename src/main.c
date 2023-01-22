#include "chip8.h"
#include "consts.h"
#include "debug.h"
#include "io.h"
#include "structs.h"
#include "args.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    uint64_t delay, timer_cycle_rate;
    Display display;
    Chip8 chip;
    Debugger *debugger = NULL;
    bool quit = false;
    struct timespec now, last_cycle;
    uint64_t diff = 0;
    int timer_counter = 0;

    if (argc < 2) {
        usage();
        return 1;
    }

    Args args = {0};
    if (read_args(&args, argc, argv)) {
        return 1;
    }

    if (args.help) {
        usage();
        return 0;
    }

    if (args.debug) {
        debugger = malloc(sizeof(Debugger));
        if (debugger == NULL) {
            printf("ERROR: Failed to assign memory for debugger!\n");
            return 1;
        }
        init_debugger(debugger);
    }

    init_chip8(&chip, debugger, args.foreground, args.background);

    if (load_rom(&chip, args.rom)) {
        // Exit because we found an error
        printf("Exiting\n");
        return 1;
    }

    // Startup SDL
    if (!init_display(&display, args.scale)) {
        printf("Failed to initialize display\n");
        return 1;
    }

    update_display(&display, &chip.screen, PITCH);

    // Calculate CPU timing 
    delay = args.target_cycles / 1000000;
    timer_cycle_rate = args.target_cycles / 60;

    clock_gettime(CLOCK_MONOTONIC_RAW, &last_cycle);
    while (!quit) {
        quit = process_keyboard_input(&chip);
        // This is needed for ESC to work during debug mode
        if (quit) continue;
        
        clock_gettime(CLOCK_MONOTONIC_RAW, &now);
        diff = (now.tv_sec - last_cycle.tv_sec) * 1000000 +
               (now.tv_nsec - last_cycle.tv_sec) / 1000;

        if (diff > delay) {
            last_cycle = now;

            if (chip.debugger != NULL) {
                quit = chip.debugger->exit;
                if (!quit && chip.debugger->stepping) {
                    quit = debug_prompt_user(chip.debugger, &chip);
                }

                // Prevent execution of instruction when we quit the debugger
                if (quit) continue;
            }
            
            // Execute a single instruction and update the display as needed
            if (cycle(&chip)) update_display(&display, &chip.screen, PITCH);

            timer_counter++;
            // 60Hz Loop
            if (timer_counter == timer_cycle_rate) {
                update_timers(&chip);
                timer_counter = 0;
            }
        }
    }

    // Free and close SDL and Chip8
    cleanup_display(&display);
    cleanup_chip8(&chip);

    return 0;
}
