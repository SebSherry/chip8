#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <stdbool.h>
#include "io.h"
#include "consts.h"
#include "structs.h"

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

bool process_keyboard_input(Chip8 *chip) {
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
                    }
                    case SDLK_1:
                    {
                        chip->keys_pressed[0x1] = true;
                        break;
                    } 
                    case SDLK_2:
                    {
                        chip->keys_pressed[0x2] = true;
                        break;
                    } 
                    case SDLK_3:
                    {
                        chip->keys_pressed[0x3] = true;
                        break;
                    } 
                    case SDLK_4:
                    {
                        chip->keys_pressed[0xC] = true;
                        break;
                    } 
                    case SDLK_q:
                    {
                        chip->keys_pressed[0x4] = true;
                        break;
                    } 
                    case SDLK_w:
                    {
                        chip->keys_pressed[0x5] = true;
                        break;
                    } 
                    case SDLK_e:
                    {
                        chip->keys_pressed[0x6] = true;
                        break;
                    } 
                    case SDLK_r:
                    {
                        chip->keys_pressed[0xD] = true;
                        break;
                    } 
                    case SDLK_a:
                    {
                        chip->keys_pressed[0x7] = true;
                        break;
                    } 
                    case SDLK_s:
                    {
                        chip->keys_pressed[0x8] = true;
                        break;
                    } 
                    case SDLK_d:
                    {
                        chip->keys_pressed[0x9] = true;
                        break;
                    } 
                    case SDLK_f:
                    {
                        chip->keys_pressed[0xE] = true;
                        break;
                    } 
                    case SDLK_z:
                    {
                        chip->keys_pressed[0xA] = true;
                        break;
                    } 
                    case SDLK_x:
                    {
                        chip->keys_pressed[0x0] = true;
                        break;
                    } 
                    case SDLK_c:
                    {
                        chip->keys_pressed[0xB] = true;
                        break;
                    } 
                    case SDLK_v:
                    {
                        chip->keys_pressed[0xF] = true;
                        break;
                    } break;
                }    
            } break;
                 
            case SDL_KEYUP:
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_1:
                    {
                        chip->keys_pressed[0x1] = false;
                        break;
                    } 
                    case SDLK_2:
                    {
                        chip->keys_pressed[0x2] = false;
                        break;
                    } 
                    case SDLK_3:
                    {
                        chip->keys_pressed[0x3] = false;
                        break;
                    } 
                    case SDLK_4:
                    {
                        chip->keys_pressed[0xC] = false;
                        break;
                    } 
                    case SDLK_q:
                    {
                        chip->keys_pressed[0x4] = false;
                        break;
                    } 
                    case SDLK_w:
                    {
                        chip->keys_pressed[0x5] = false;
                        break;
                    } 
                    case SDLK_e:
                    {
                        chip->keys_pressed[0x6] = false;
                        break;
                    } 
                    case SDLK_r:
                    {
                        chip->keys_pressed[0xD] = false;
                        break;
                    } 
                    case SDLK_a:
                    {
                        chip->keys_pressed[0x7] = false;
                        break;
                    } 
                    case SDLK_s:
                    {
                        chip->keys_pressed[0x8] = false;
                        break;
                    } 
                    case SDLK_d:
                    {
                        chip->keys_pressed[0x9] = false;
                        break;
                    } 
                    case SDLK_f:
                    {
                        chip->keys_pressed[0xE] = false;
                        break;
                    } 
                    case SDLK_z:
                    {
                        chip->keys_pressed[0xA] = false;
                        break;
                    } 
                    case SDLK_x:
                    {
                        chip->keys_pressed[0x0] = false;
                        break;
                    } 
                    case SDLK_c:
                    {
                        chip->keys_pressed[0xB] = false;
                        break;
                    } 
                    case SDLK_v:
                    {
                        chip->keys_pressed[0xF] = false;
                        break;
                    } break;
                }    
            } break;    
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

