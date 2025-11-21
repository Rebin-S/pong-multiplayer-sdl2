#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL2/SDL.h>

void init_background(SDL_Renderer* renderer, int screenWidth, int screenHeight);
void render_background(SDL_Renderer* renderer);
void cleanup_background();

#endif
