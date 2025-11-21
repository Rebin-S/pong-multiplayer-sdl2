#ifndef PONG_DATA_H
#define PONG_DATA_H

#include <stdbool.h>
#include "ball_state.h"
#include "paddle_data.h"
#include "score_data.h"

typedef enum {
    MOVE_UP,
    MOVE_DOWN
} CommandType;

typedef enum {
    WAITING,
    ONGOING
} GameState;

typedef struct {
    int playerId;
    CommandType command;
    int difficulty;
    int pointsToWin;
    bool isOption;
} ClientData;

typedef struct {
    int playerId;
    GameState gState;
    BallState ballState;    
    Paddle paddles[4];
    Score score;
    int frameNumber;
    bool gameOver;
    int winner;
} ServerData;

#endif
