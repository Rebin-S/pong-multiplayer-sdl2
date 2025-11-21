#ifndef _MENU_H
#define _MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

int showClientMenu(SDL_Renderer *renderer, TTF_Font *font, char *ipBuffer);
int showServerMenu(SDL_Renderer *renderer, TTF_Font *font, int *mode, int *points);

#endif
