#include "chip8.h"
#include "debug.h"
#include "io.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef struct {
  char *rom;
  int debug;
} Args;

void usage() { printf("Usage: ./chip8 [--debug] <rom file>\n"); }

int read_args(Args *args, int argc, char *argv[]) {
  args->rom = NULL;
  args->debug = 0;

  for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
          if (strcmp(argv[i], "--debug") || strcmp(argv[i], "-d")) {
              args->debug = 1;
          } else {
              printf("Invalid option: %s", argv[i]);
              return 1;
          }
      } else if (args->rom == NULL) {
          args->rom = argv[i];
      }
  }

  return 0;
}

int main(int argc, char *argv[]) {
    int scale = 10;
    int target_cycle_rate = 700;
    uint64_t delay = target_cycle_rate / 1000000;
    Display display;
    Chip8 chip;
    bool quit = false;
    struct timespec now, last_cycle;
    uint64_t diff = 0;
    int pitch = 0;

    if (argc < 2) {
        usage();
        return 1;
    }

    Args args = {0};
    if (read_args(&args, argc, argv)) {
        return 1;
    }

    init_chip8(&chip, args.debug);
    if (load_rom(&chip, args.rom)) {
        // Exit because we found an error
        printf("Exiting\n");
        return 1;
    }

    pitch = sizeof(chip.screen[0]) * SCREEN_WIDTH;

    // Startup SDL
    if (!init_display(&display, scale)) {
        printf("Failed to initialize display\n");
        return 1;
    }

    update_display(&display, &chip.screen, pitch);

    clock_gettime(CLOCK_MONOTONIC_RAW, &last_cycle);
    while (!quit) {
        quit = process_keyboard_input();
        clock_gettime(CLOCK_MONOTONIC_RAW, &now);
        diff = (now.tv_sec - last_cycle.tv_sec) * 1000000 +
               (now.tv_nsec - last_cycle.tv_sec) / 1000;

        if (diff > delay) {
            last_cycle = now;

            cycle(&chip);

            update_display(&display, &chip.screen, pitch);

            if (chip.debug) {
                quit = debug_prompt_user();
            }
        }
    }

    // Free and close SDL
    cleanup_display(&display);

    return 0;
}
