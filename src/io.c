#include <SDL2/SDL.h>
#include <stdbool.h>
#include "io.h"

bool init_display(Display *display, int scale) {
  // Init SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return false;
  } 
  
  // Create window
  display->window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * scale,
                           SCREEN_HEIGHT * scale, SDL_WINDOW_SHOWN);

  if (display->window == NULL) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return false;
  } 

  // Create Renderer
  display->renderer = SDL_CreateRenderer(display->window, -1, SDL_RENDERER_ACCELERATED);
      
  if (display->renderer == NULL) {
    printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  // Create Texture
  display->texture = SDL_CreateTexture(display->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

  if (display->texture == NULL) {
    printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
    return false;
  }
  

  return true;
}

void update_display(Display *display, void const* buffer, int pitch) {
  SDL_UpdateTexture(display->texture, NULL, buffer, pitch);
  SDL_RenderClear(display->renderer);
  SDL_RenderCopy(display->renderer, display->texture, NULL, NULL);
  SDL_RenderPresent(display->renderer);
}

bool process_keyboard_input() {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT: {
        return true;
      } break;

      case SDL_KEYDOWN:
      {
        switch(event.key.keysym.sym)
        {
          case SDLK_ESCAPE:
          {
              return true;      
          } break;
        }    
      }     
    } 
  }

  return false;
}

void cleanup_display(Display *display) {
  SDL_DestroyTexture(display->texture);
  display->texture = NULL;
  
  SDL_DestroyRenderer(display->renderer);
  display->renderer = NULL;

  // Destroy window
  SDL_DestroyWindow(display->window);
  display->window = NULL;

  // Quit SDL subsystems
  SDL_Quit();
}

