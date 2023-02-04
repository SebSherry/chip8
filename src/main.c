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
    uint64_t delay, cycles_per_frame;
    Display display;
    Chip8 chip;
    Debugger *debugger = NULL;
    bool quit = false;
    struct timespec now, last_cycle;
    uint64_t diff = 0;

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
    delay = MICROSECS_IN_SECOND / TARGET_FRAMES_PER_SECOND;
    cycles_per_frame = args.target_cycles / TARGET_FRAMES_PER_SECOND;

    clock_gettime(CLOCK_MONOTONIC_RAW, &last_cycle);
    while (!quit) {
        quit = process_keyboard_input(&chip);
        // This is needed for ESC to work during debug mode
        if (quit) continue;
        
        clock_gettime(CLOCK_MONOTONIC_RAW, &now);
        diff = (now.tv_sec - last_cycle.tv_sec) * MICROSECS_IN_SECOND +
               (now.tv_nsec - last_cycle.tv_nsec) / NANOSECS_IN_MICROSECONDS;

        if (diff > delay) {
            last_cycle = now;

            update_display(&display, &chip.screen, PITCH);
            update_timers(&chip);

            if (chip.waiting_to_draw > 2) {
                chip.display_interrupt_triggered = true;
            }

            for (int i = 0; i < cycles_per_frame; i++) {
                if (chip.debugger != NULL) {
                    quit = chip.debugger->exit;
                    if (!quit && chip.debugger->stepping) {
                        quit = debug_prompt_user(chip.debugger, &chip);
                    }

                    // Prevent execution of instruction when we quit the debugger
                    if (quit) break;
                }
                cycle(&chip);
                
            } 
        }
    }

    // Free and close SDL and Chip8
    cleanup_display(&display);
    cleanup_chip8(&chip);

    return 0;
}
