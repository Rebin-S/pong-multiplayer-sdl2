#ifndef PADDLE_H
#define PADDLE_H

#include <SDL2/SDL.h>
#include "paddle_data.h"

Paddle init_paddle(int x, int y, SDL_Color color, int modeChoice);
void update_paddles(Paddle paddles[], int screenHeight);
void handle_paddle_input(Paddle paddles[], SDL_Event e);

#endif
