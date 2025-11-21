#ifndef BALL_H
#define BALL_H

#include <stdbool.h>
#include "score_data.h"
#include "paddle_data.h"
#include "ball_state.h"  

typedef struct Ball Ball;  

Ball* create_ball(int modeChoice);
void destroy_ball(Ball* b);

bool update_ball(Ball* b, Score* score, int screenWidth, int screenHeight, Paddle paddles[], int maxScore);
void normalizeBallSpeed(Ball* b, float speed);
void copy_ball(Ball* dest, const Ball* src);
void update_ball_from_state(Ball* b, BallState bs);

float get_ball_speedX(Ball* b);
float get_ball_speedY(Ball* b);

int get_ball_x(Ball* b);
int get_ball_y(Ball* b);
int get_ball_w(Ball* b);
int get_ball_h(Ball* b);

#endif
