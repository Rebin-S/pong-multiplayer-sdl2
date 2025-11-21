#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include "pong_data.h"
#include "background.h"
#include "score.h"
#include "menu.h"
#include "music.h"
#include "ball.h"
#include "ball_state.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

void render_winner_screen(SDL_Renderer *renderer, TTF_Font *font, int winner);

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;

    UDPsocket socket;
    UDPpacket *packet;
    IPaddress serverAddr;

    Ball *ball;
    Paddle paddles[4];
    Score score;

    int playerId;
    GameState gState;
    SDL_Texture *ballTex;
    int latestFrame;
} GameClient;

void render_ball(SDL_Renderer *r, Ball *b, SDL_Texture *t)
{
    SDL_Rect dst = {
        get_ball_x(b),
        get_ball_y(b),
        get_ball_w(b),
        get_ball_h(b)};

    SDL_RenderCopy(r, t, NULL, &dst);
}

void render_paddle(SDL_Renderer *r, Paddle *p)
{
    SDL_Rect rc = {p->x, p->y, p->w, p->h};
    SDL_SetRenderDrawColor(r, p->r, p->g, p->b, 255);
    SDL_RenderFillRect(r, &rc);
}

void render_waiting_message(SDL_Renderer *renderer, TTF_Font *font)
{
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, "Waiting for next match...", white);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {SCREEN_WIDTH / 2 - surface->w / 2, SCREEN_HEIGHT / 2 - surface->h / 2, surface->w, surface->h};

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_RenderPresent(renderer);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

bool connectToServer(const char *ip, GameClient *c)
{
    printf("CLIENT: Forsoker ansluta till server pa IP %s...\n", ip);

    if (SDLNet_ResolveHost(&c->serverAddr, ip, 5005) < 0)
    {
        printf("CLIENT: SDLNet_ResolveHost misslyckades: %s\n", SDLNet_GetError());
        return false;
    }

    c->packet->address = c->serverAddr;
    c->packet->len = 1;
    c->packet->data[0] = 0;
    SDLNet_UDP_Send(c->socket, -1, c->packet);

    Uint32 start = SDL_GetTicks();
    while (SDL_GetTicks() - start < 5000)
    {
        if (SDLNet_UDP_Recv(c->socket, c->packet))
        {
            printf("CLIENT: Fick svar fran server: %s\n", (char *)c->packet->data);
            if (strcmp((char *)c->packet->data, "OK") == 0)
                return true;
        }
    }

    printf("CLIENT: Timeout - ingen respons fran server inom 5 sekunder.\n");
    return false;
}

void sendCommand(GameClient *c, CommandType cmd)
{
    ClientData cd = {.playerId = c->playerId, .command = cmd};
    memcpy(c->packet->data, &cd, sizeof(cd));
    c->packet->len = sizeof(cd);
    c->packet->address = c->serverAddr;
    SDLNet_UDP_Send(c->socket, -1, c->packet);
}

void updateFromServer(GameClient *c)
{
    ServerData sd;
    bool got = false;
    while (SDLNet_UDP_Recv(c->socket, c->packet))
    {
        memcpy(&sd, c->packet->data, sizeof(sd));
        got = true;
    }

    if (!got || sd.frameNumber <= c->latestFrame)
        return;

    c->latestFrame = sd.frameNumber;

    update_ball_from_state(c->ball, sd.ballState);

    c->score = sd.score;
    for (int i = 0; i < 4; i++)
    {
        c->paddles[i] = sd.paddles[i];
    }

    c->playerId = sd.playerId;
    c->gState = sd.gState;

    if (sd.gameOver)
    {
        printf("Game Over mottaget!\n");

        render_winner_screen(c->renderer, c->font, sd.winner);

        c->gState = WAITING;
    }
}

void showCountdown(SDL_Renderer *r, TTF_Font *f)
{
    SDL_Color white = {255, 255, 255};
    char txt[2];
    for (int i = 3; i > 0; i--)
    {
        snprintf(txt, sizeof(txt), "%d", i);
        SDL_Surface *s = TTF_RenderText_Solid(f, txt, white);
        SDL_Texture *t = SDL_CreateTextureFromSurface(r, s);
        SDL_Rect rc = {SCREEN_WIDTH / 2 - s->w / 2, SCREEN_HEIGHT / 2 - s->h / 2, s->w, s->h};
        SDL_RenderClear(r);
        SDL_RenderCopy(r, t, NULL, &rc);
        SDL_RenderPresent(r);
        SDL_FreeSurface(s);
        SDL_DestroyTexture(t);
        SDL_Delay(1000);
    }
}

void handleInput(GameClient *c, SDL_Event e)
{
    if (e.type == SDL_KEYDOWN)
    {
        if (e.key.keysym.scancode == SDL_SCANCODE_UP &&
            c->playerId >= 0 && c->playerId < 4)
        {
            c->paddles[c->playerId].y -= c->paddles[c->playerId].speed;
            if (c->paddles[c->playerId].y < 0)
                c->paddles[c->playerId].y = 0;
            sendCommand(c, MOVE_UP);
        }
        else if (e.key.keysym.scancode == SDL_SCANCODE_DOWN &&
                 c->playerId >= 0 && c->playerId < 4)
        {
            c->paddles[c->playerId].y += c->paddles[c->playerId].speed;
            if (c->paddles[c->playerId].y > SCREEN_HEIGHT - c->paddles[c->playerId].h)
                c->paddles[c->playerId].y = SCREEN_HEIGHT - c->paddles[c->playerId].h;

            sendCommand(c, MOVE_DOWN);
        }
    }
}


int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    SDLNet_Init();
    initMusicSystem();
    playMusic("uclmusic.mp3");

    GameClient c = {0};
    c.latestFrame = -1;

    c.window = SDL_CreateWindow("Pong Client",
                                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    c.renderer = SDL_CreateRenderer(c.window, -1,
                                    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    c.font = TTF_OpenFont("assets/assets2/DejaVuSans.ttf", 24);

    char ipb[16] = "";
    if (showClientMenu(c.renderer, c.font, ipb) != 0)
    {
        SDL_Quit();
        return 0;
    }

    c.socket = SDLNet_UDP_Open(0);
    c.packet = SDLNet_AllocPacket(512);

    if (!connectToServer(ipb, &c))
    {
        printf("CLIENT: Kunde inte ansluta - stanger ner.\n");
        return 1;
    }

    ClientData rd = {.playerId = -1, .command = MOVE_UP};
    memcpy(c.packet->data, &rd, sizeof(rd));
    c.packet->len = sizeof(rd);
    c.packet->address = c.serverAddr;
    SDLNet_UDP_Send(c.socket, -1, c.packet);
    printf("CLIENT: Skickade READY till server.\n");

    c.ball = create_ball(1); 
    if (!c.ball)
    {
        printf("CLIENT: Misslyckades att skapa boll. Avslutar.\n");
        return 1;
    }

    SDL_Surface *surf = IMG_Load("assets/bollen.png");
    c.ballTex = SDL_CreateTextureFromSurface(c.renderer, surf);
    SDL_FreeSurface(surf);
    init_background(c.renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Event e;
    bool run = true;

    while (run)
    {
        while (c.gState != ONGOING && run)
        {
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT)
                    run = false;
            }
            updateFromServer(&c);
            render_waiting_message(c.renderer, c.font);
            if (c.gState == ONGOING)
            {
                showCountdown(c.renderer, c.font);
                break;
            }
            SDL_Delay(16);
        }

        while (c.gState == ONGOING && run)
        {
            updateFromServer(&c);
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT)
                    run = false;
                handleInput(&c, e);
            }

            SDL_SetRenderDrawColor(c.renderer, 0, 0, 0, 255);
            SDL_RenderClear(c.renderer);
            render_background(c.renderer);
            render_ball(c.renderer, c.ball, c.ballTex);
            for (int i = 0; i < 4; i++)
                render_paddle(c.renderer, &c.paddles[i]);
            draw_scores(c.renderer, c.font, c.score.left, c.score.right, SCREEN_WIDTH);
            SDL_RenderPresent(c.renderer);
            SDL_Delay(16);
        }
    }

    SDL_DestroyTexture(c.ballTex);
    SDLNet_FreePacket(c.packet);
    SDLNet_UDP_Close(c.socket);
    TTF_CloseFont(c.font);
    SDL_DestroyRenderer(c.renderer);
    SDL_DestroyWindow(c.window);
    cleanup_background();
    SDLNet_Quit();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    stopMusic();
    return 0;
}

void render_winner_screen(SDL_Renderer *renderer, TTF_Font *font, int winner)
{
    const char *text = (winner == 1) ? "Red Team Wins!" : "Blue Team Wins!";
    SDL_Color color = (winner == 1) ? (SDL_Color){255, 0, 0, 255} : (SDL_Color){0, 0, 255, 255};

    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect = {
        SCREEN_WIDTH / 2 - surface->w / 2,
        SCREEN_HEIGHT / 2 - surface->h / 2,
        surface->w, surface->h};

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_RenderPresent(renderer);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_Delay(5000); 
}
