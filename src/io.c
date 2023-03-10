#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "io.h"
#include "consts.h"

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
        if (event.type == SDL_QUIT) {
            return true;
            break;
        } 

        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                return true;
            }   

            // We ignore invalid keys here
            register_key_press(chip, event.key.keysym.sym);
        }   
    }
    
    return false;
}

bool register_key_press(Chip8 *chip, int key) {
    bool valid = true;
    
    switch(key) {
        case SDLK_1:
            chip->keys_pressed ^= KEY_1_MASK;
            break;
        case SDLK_2:
            chip->keys_pressed ^= KEY_2_MASK;
            break;
        case SDLK_3:
            chip->keys_pressed ^= KEY_3_MASK;
            break;
        case SDLK_4:
            chip->keys_pressed ^= KEY_C_MASK;
            break;
        case SDLK_q:
            chip->keys_pressed ^= KEY_4_MASK;
            break;
        case SDLK_w:
            chip->keys_pressed ^= KEY_5_MASK;
            break;
        case SDLK_e:
            chip->keys_pressed ^= KEY_6_MASK;
            break;
        case SDLK_r:
            chip->keys_pressed ^= KEY_D_MASK;
            break;
        case SDLK_a:
            chip->keys_pressed ^= KEY_7_MASK;
            break;
        case SDLK_s:
            chip->keys_pressed ^= KEY_8_MASK;
            break;
        case SDLK_d:
            chip->keys_pressed ^= KEY_9_MASK;
            break;
        case SDLK_f:
            chip->keys_pressed ^= KEY_E_MASK;
            break;
        case SDLK_z:
            chip->keys_pressed ^= KEY_A_MASK;
            break;
        case SDLK_x:
            chip->keys_pressed ^= KEY_0_MASK;
            break;
        case SDLK_c:
            chip->keys_pressed ^= KEY_B_MASK;
            break;
        case SDLK_v:
            chip->keys_pressed ^= KEY_F_MASK;
            break;
        default:
            valid = false; 
    }    

    return valid;
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

int read_user_character_input() {
    char *line = NULL;
    size_t len = 0;
    
    // We only want 1 character, so anything other than 1 is a failure
    if (getline(&line, &len, stdin) != 2) {
        free(line);
        return -1;
    }

    char character;
    sscanf(line, "%c", &character);
    
    free(line);
    return character;
}

int read_user_integer_input() {
    char *line = NULL;
    size_t len = 0;
    
    if (getline(&line, &len, stdin) == -1) {
        free(line);
        return -1;
    }

    int number;
    char *end = NULL;

    number = strtoul(line, &end, 10);

    // Fail on Leftover input
    if (strncmp(end, "\n", 1)) {
        return -1;
    }

    free(line);
    return number;
}

