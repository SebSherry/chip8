#ifndef IO_H_
#define IO_H_

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "structs.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Display;

bool init_display(Display *display, int scale);
void update_display(Display *display, void const* buffer, int pitch);
bool process_keyboard_input(Chip8 *chip);
bool register_key_press(Chip8 *chip, int key);
void cleanup_display(Display *display);

#endif
