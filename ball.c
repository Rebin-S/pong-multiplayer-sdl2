#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>
#include "ball.h"
#include "pong_data.h"

#define SPEED1 5
#define SPEED2 9
#define SPEED3 15

struct Ball {
    int x, y;
    int w, h;
    float speedX, speedY;
    float initialSpeed;
    float normalizedTargetSpeed;
};

Ball* create_ball(int modeChoice)
{
    Ball* b = malloc(sizeof(Ball));
    if (!b) return NULL;

    b->x = 400;
    b->y = 300;

    if (modeChoice == 1) {
        b->w = 64;
        b->h = 64;
        b->initialSpeed = SPEED1;
        b->normalizedTargetSpeed = 13.0f;
    } else if (modeChoice == 2) {
        b->w = 50;
        b->h = 50;
        b->initialSpeed = SPEED2;
        b->normalizedTargetSpeed = 18.0f;
    } else {
        b->w = 45;
        b->h = 45;
        b->initialSpeed = SPEED3;
        b->normalizedTargetSpeed = 21.0f;
    }

    b->speedX = b->initialSpeed;
    b->speedY = b->initialSpeed;

    return b;
}

void destroy_ball(Ball* b)
{
    free(b);
}

void normalizeBallSpeed(Ball *b, float speed)
{
    float currentSpeed = sqrtf(b->speedX * b->speedX + b->speedY * b->speedY);
    if (currentSpeed == 0) return;

    b->speedX = (b->speedX / currentSpeed) * speed;
    b->speedY = (b->speedY / currentSpeed) * speed;
}

bool update_ball(Ball *b, Score *score, int screenWidth, int screenHeight, Paddle paddles[], int maxScore)
{
    b->x += b->speedX;
    b->y += b->speedY;

    if (b->y <= 0 || b->y + b->h >= screenHeight) {
        b->speedY *= -1;
        normalizeBallSpeed(b, b->normalizedTargetSpeed);
    }

    for (int i = 0; i < 4; i++) {
        SDL_Rect ballRect = {b->x, b->y, b->w, b->h};
        SDL_Rect paddleRect = {paddles[i].x, paddles[i].y, paddles[i].w, paddles[i].h};
        if (SDL_HasIntersection(&ballRect, &paddleRect)) {
            float relativeIntersectY = (paddles[i].y + paddles[i].h / 2) - (b->y + b->h / 2);
            float normalizedY = relativeIntersectY / (paddles[i].h / 2);
            float bounceAngle = normalizedY * (75.0f * (3.14159f / 180.0f));
            float ballSpeed = sqrtf(b->speedX * b->speedX + b->speedY * b->speedY);
            int direction = (b->speedX > 0) ? -1 : 1;

            b->speedX = ballSpeed * cosf(bounceAngle) * direction;
            b->speedY = ballSpeed * -sinf(bounceAngle);
            normalizeBallSpeed(b, b->normalizedTargetSpeed);
            break;
        }
    }

    if (b->x + b->w < 0) {
        score->right++;
        b->x = screenWidth / 2;
        b->y = screenHeight / 2;
        b->speedX = -b->initialSpeed;
        b->speedY = b->initialSpeed;
        normalizeBallSpeed(b, b->normalizedTargetSpeed);
    }

    if (b->x > screenWidth) {
        score->left++;
        b->x = screenWidth / 2;
        b->y = screenHeight / 2;
        b->speedX = b->initialSpeed;
        b->speedY = b->initialSpeed;
        normalizeBallSpeed(b, b->normalizedTargetSpeed);
    }

    return (score->left >= maxScore || score->right >= maxScore);
}

void copy_ball(Ball* dest, const Ball* src) {
    *dest = *src;
}

void update_ball_from_state(Ball* b, BallState bs)
{
    b->x = bs.x;
    b->y = bs.y;
    b->w = bs.w;
    b->h = bs.h;
    b->speedX = bs.speedX;
    b->speedY = bs.speedY;
}

float get_ball_speedX(Ball* b) { return b->speedX; }
float get_ball_speedY(Ball* b) { return b->speedY; }

int get_ball_x(Ball* b) { return b->x; }
int get_ball_y(Ball* b) { return b->y; }
int get_ball_w(Ball* b) { return b->w; }
int get_ball_h(Ball* b) { return b->h; }
