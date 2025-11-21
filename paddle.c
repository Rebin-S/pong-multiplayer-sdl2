#include "paddle.h"
#include <SDL2/SDL.h>

#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 100

Paddle init_paddle(int x, int y, SDL_Color color, int modeChoice) {
    Paddle p;
    p.x = x;
    p.y = y;
    p.w = PADDLE_WIDTH;

    if (modeChoice == 1 || modeChoice == 3)
        p.h = PADDLE_HEIGHT;
    else
        p.h = PADDLE_HEIGHT / 1.2;

    p.speed = 10;
    p.r = color.r;
    p.g = color.g;
    p.b = color.b;

    return p;
}

void update_paddles(Paddle paddles[], int screenHeight) {
    for (int i = 0; i < 4; i++) {
        if (paddles[i].y < 0)
            paddles[i].y = 0;
        if (paddles[i].y + paddles[i].h > screenHeight)
            paddles[i].y = screenHeight - paddles[i].h;
    }
}
