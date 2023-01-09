#ifndef IO_H_
#define IO_H_

// Consts
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)
#define PIXEL_BLACK 0
#define PIXEL_WHITE 0xFFFFFFFF

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Display;

bool init_display(Display *display, int scale);
void update_display(Display *display, void const* buffer, int pitch);
bool process_keyboard_input();
void cleanup_display(Display *display);

#endif
