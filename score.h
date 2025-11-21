#ifndef SCORE_H
#define SCORE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "score_data.h"

TTF_Font* load_font(const char* path, int size);
Score init_score();
void draw_scores(SDL_Renderer* renderer, TTF_Font* font, int team1_score, int team2_score, int screenWidth);
void show_game_over(SDL_Renderer* renderer, TTF_Font* font, int winner);

#endif
