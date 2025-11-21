#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "ball.h"
#include "paddle.h"
#include "score.h"
#include "pong_data.h"
#include "background.h"
#include "menu.h"
#include "music.h"

#define MAX_PLAYERS 4
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

typedef struct
{
    UDPsocket socket;
    UDPpacket *packet;
    IPaddress clients[MAX_PLAYERS];
    bool hasReceivedStart[MAX_PLAYERS];
    int connectedPlayers;

    Ball *ball;
    Paddle paddles[4];
    Score score;

    int pointsToWin;
    int frameNumber;
    bool gameStarted;
    bool gameOver;
} GameServer;

void resetGameServer(GameServer *gs)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        gs->clients[i].host = 0;
        gs->clients[i].port = 0;
        gs->hasReceivedStart[i] = false;
    }
    gs->connectedPlayers = 0;
    gs->gameStarted = false;
    gs->gameOver = false;
    gs->frameNumber = 0;
}

void show_restart_menu(SDL_Renderer *renderer, TTF_Font *font)
{
    SDL_Color white = {255, 255, 255, 255};

    SDL_Surface *surface2 = TTF_RenderText_Solid(font, "To exit the game, press E", white);

    SDL_Texture *texture2 = SDL_CreateTextureFromSurface(renderer, surface2);

    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

    SDL_Rect rect1 = {0}, rect2 = {0};
    rect2.w = surface2->w;
    rect2.h = surface2->h;

    int padding = 50;
    int widestWidth = (rect1.w > rect2.w) ? rect1.w : rect2.w;
    SDL_Rect backgroundRect = {
        .w = widestWidth + padding * 2,
        .h = rect1.h + rect2.h + padding * 3,
        .x = (screenWidth - widestWidth - padding * 2) / 2,
        .y = (screenHeight - (rect1.h + rect2.h + padding * 3)) / 2};

    rect1.x = backgroundRect.x + (backgroundRect.w - rect1.w) / 2;
    rect1.y = backgroundRect.y + padding;
    rect2.x = backgroundRect.x + (backgroundRect.w - rect2.w) / 2;
    rect2.y = rect1.y + rect1.h + padding / 2;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 200 + rand() % 55, 255, 220);
    SDL_RenderFillRect(renderer, &backgroundRect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &backgroundRect);

    SDL_RenderCopy(renderer, texture2, NULL, &rect2);
    SDL_RenderPresent(renderer);

    SDL_FreeSurface(surface2);
    SDL_DestroyTexture(texture2);
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDLNet_Init();
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    SDL_Window *menuWindow = SDL_CreateWindow("Server Setup",
                                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                              SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *menuRenderer = SDL_CreateRenderer(menuWindow, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont("assets/assets2/DejaVuSans.ttf", 24);

    bool running = true;
    while (running)
    {
        int mode = 1, points = 1;
        if (showServerMenu(menuRenderer, font, &mode, &points) != 0)
        {
            printf("Server menu cancelled. Exiting...\n");
            break;
        }

        GameServer gs = {0};
        resetGameServer(&gs);

        gs.pointsToWin = (points == 1) ? 7 : 11;
        gs.socket = SDLNet_UDP_Open(5005);
        gs.packet = SDLNet_AllocPacket(512);

        gs.ball = create_ball(mode);
        gs.score = init_score();
        SDL_Color red = {255, 0, 0, 255}, blue = {0, 0, 255, 255};
        gs.paddles[0] = init_paddle(50, SCREEN_HEIGHT / 3, red, mode);
        gs.paddles[1] = init_paddle(50, 2 * SCREEN_HEIGHT / 3, red, mode);
        gs.paddles[2] = init_paddle(SCREEN_WIDTH - 70, SCREEN_HEIGHT / 3, blue, mode);
        gs.paddles[3] = init_paddle(SCREEN_WIDTH - 70, 2 * SCREEN_HEIGHT / 3, blue, mode);

        while (true)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    running = false;
                    break;
                }
            }
            while (SDLNet_UDP_Recv(gs.socket, gs.packet))
            {
                if (gs.packet->len == 1 && gs.packet->data[0] == 0)
                {
                    bool alreadyConnected = false;
                    for (int i = 0; i < gs.connectedPlayers; i++)
                    {
                        if (gs.packet->address.host == gs.clients[i].host &&
                            gs.packet->address.port == gs.clients[i].port)
                        {
                            alreadyConnected = true;
                            break;
                        }
                    }

                    if (!alreadyConnected && gs.connectedPlayers < MAX_PLAYERS)
                    {
                        gs.clients[gs.connectedPlayers] = gs.packet->address;
                        gs.hasReceivedStart[gs.connectedPlayers] = false;
                        gs.connectedPlayers++;
                        printf("Client %d connected!\n", gs.connectedPlayers);

                        UDPpacket *ok = SDLNet_AllocPacket(512);
                        strcpy((char *)ok->data, "OK");
                        ok->len = strlen("OK") + 1;
                        ok->address = gs.packet->address;
                        SDLNet_UDP_Send(gs.socket, -1, ok);
                        SDLNet_FreePacket(ok);
                    }
                    else if (!alreadyConnected)
                    {
                        printf("En extra klient forsokte ansluta men servern ar full.\n");
                        UDPpacket *full = SDLNet_AllocPacket(512);
                        strcpy((char *)full->data, "FULL");
                        full->len = strlen("FULL") + 1;
                        full->address = gs.packet->address;
                        SDLNet_UDP_Send(gs.socket, -1, full);
                        SDLNet_FreePacket(full);
                    }
                }

                else
                {
                    ClientData cd;
                    memcpy(&cd, gs.packet->data, sizeof(cd));
                    for (int i = 0; i < gs.connectedPlayers; i++)
                    {
                        if (gs.packet->address.host == gs.clients[i].host && gs.packet->address.port == gs.clients[i].port)
                        {
                            if (!gs.hasReceivedStart[i] && cd.playerId == -1)
                            {
                                gs.hasReceivedStart[i] = true;
                                printf("Client %d READY.\n", i + 1);
                                break;
                            }
                            int id = cd.playerId;
                            if (id >= 0 && id < MAX_PLAYERS)
                            {
                                if (cd.command == MOVE_UP)
                                    gs.paddles[id].y -= gs.paddles[id].speed;
                                else
                                    gs.paddles[id].y += gs.paddles[id].speed;
                                if (gs.paddles[id].y < 0)
                                    gs.paddles[id].y = 0;
                                if (gs.paddles[id].y > SCREEN_HEIGHT - gs.paddles[id].h)
                                    gs.paddles[id].y = SCREEN_HEIGHT - gs.paddles[id].h;
                            }
                        }
                    }
                }
            }

            static bool printedReady[MAX_PLAYERS] = {false};

            if (!gs.gameStarted)
            {
                bool allReady = true;

                if (gs.connectedPlayers < MAX_PLAYERS)
                    allReady = false;

                for (int i = 0; i < gs.connectedPlayers; i++)
                {
                    if (!printedReady[i] && gs.hasReceivedStart[i])
                    {
                        printf("Client %d READY.\n", i + 1);
                        printedReady[i] = true;
                    }

                    if (!gs.hasReceivedStart[i])
                        allReady = false;
                }

                if (allReady)
                {
                    gs.gameStarted = true;
                    printf("Alla klienter redo – startar spel!\n");
                }
            }

            if (gs.gameStarted && !gs.gameOver)
            {
                bool fin = update_ball(gs.ball, &gs.score,
                                       SCREEN_WIDTH, SCREEN_HEIGHT,
                                       gs.paddles, gs.pointsToWin);

                update_paddles(gs.paddles, SCREEN_HEIGHT);
                if (fin)
                {
                    printf("GAME OVER! Slutpoang: RED = %d | BLUE = %d\n", gs.score.left, gs.score.right);

                    if (gs.score.left > gs.score.right)
                        printf("Vinnare: RED TEAM!\n");
                    else if (gs.score.right > gs.score.left)
                        printf("Vinnare: BLUE TEAM!\n");

                    gs.gameOver = true;
                    gs.gameStarted = false;
                }
            }

            ServerData sd;
            sd.gState = gs.gameStarted ? ONGOING : WAITING;

            Ball *b = gs.ball;
            sd.ballState.x = get_ball_x(b);
            sd.ballState.y = get_ball_y(b);
            sd.ballState.w = get_ball_w(b);
            sd.ballState.h = get_ball_h(b);
            sd.ballState.speedX = get_ball_speedX(b);
            sd.ballState.speedY = get_ball_speedY(b);

            sd.score = gs.score;
            sd.frameNumber = gs.frameNumber++;
            sd.gameOver = gs.gameOver;
            for (int i = 0; i < gs.connectedPlayers; i++)
            {
                sd.playerId = i;
                for (int j = 0; j < MAX_PLAYERS; j++)
                    sd.paddles[j] = gs.paddles[j];
                memcpy(gs.packet->data, &sd, sizeof(sd));
                gs.packet->len = sizeof(sd);
                gs.packet->address = gs.clients[i];
                SDLNet_UDP_Send(gs.socket, -1, gs.packet);
            }

            if (gs.gameOver)
            {
                show_restart_menu(menuRenderer, font);
                SDL_Event e;
                bool wait = true;
                while (wait && SDL_WaitEvent(&e))
                {
                    if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_e))
                    {
                        running = false;
                        wait = false;
                    }
                    else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r)
                    {
                        wait = false; 
                    }
                }
                break;
            }

            SDL_Delay(10);
        }
        destroy_ball(gs.ball);
        SDLNet_FreePacket(gs.packet);
        SDLNet_UDP_Close(gs.socket);
    }

    SDL_DestroyRenderer(menuRenderer);
    SDL_DestroyWindow(menuWindow);
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDLNet_Quit();
    SDL_Quit();
    return 0;
}