#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "io.h"
#include "chip8.h"

void usage() {
  printf("Usage: ./chip8 <rom file>\n");
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

  init_chip8(&chip);
  if (load_rom(&chip, argv[1])) {
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

  int screen_pointer = 0;
  int colour = PIXEL_WHITE;
  
  update_display(&display, &chip.screen, pitch);

  clock_gettime(CLOCK_MONOTONIC_RAW, &last_cycle);
  while (!quit) {
    quit = process_keyboard_input();
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    diff = (now.tv_sec - last_cycle.tv_sec) * 1000000 + (now.tv_nsec - last_cycle.tv_sec) / 1000;
    
    if (diff > delay) {
      last_cycle = now;

      cycle(&chip, screen_pointer, colour);
      
      update_display(&display, &chip.screen, pitch);
      
      screen_pointer++;

      if (screen_pointer == SCREEN_SIZE) {
        screen_pointer = 0;

        colour = colour == PIXEL_BLACK ? PIXEL_WHITE : PIXEL_BLACK;
      }
    }
  }

  // Free and close SDL
  cleanup_display(&display);

  return 0;
}

