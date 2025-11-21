#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL_net.h>

#include "background.h"
#include "score.h"
#include "menu.h"
#include "music.h"
#include "pong_data.h"

int wait_for_all_players(const char *ipBuffer)
{
    if (SDLNet_Init() < 0)
        return 0;

    UDPsocket sock = SDLNet_UDP_Open(0);
    if (!sock)
        return 0;

    UDPpacket *sendPacket = SDLNet_AllocPacket(512);
    UDPpacket *recPacket = SDLNet_AllocPacket(512);

    IPaddress serverAddr;
    if (SDLNet_ResolveHost(&serverAddr, ipBuffer, 5005) < 0) {
        printf("Could not resolve host: %s\n", SDLNet_GetError());
        SDLNet_FreePacket(sendPacket);
        SDLNet_FreePacket(recPacket);
        SDLNet_UDP_Close(sock);
        SDLNet_Quit();
        return 0;
    }

    const char *statusMsg = "STATUS";
    int readyCount = 0;

    printf("Waiting for all 4 players to be ready...\n");
    while (readyCount < 4)
    {
        memcpy(sendPacket->data, statusMsg, strlen(statusMsg) + 1);
        sendPacket->len = strlen(statusMsg) + 1;
        sendPacket->address = serverAddr;
        SDLNet_UDP_Send(sock, -1, sendPacket);

        if (SDLNet_UDP_Recv(sock, recPacket))
        {
            sscanf((char *)recPacket->data, "%d", &readyCount);
            printf("%d/4 players are ready...\n", readyCount);
        }

        SDL_Delay(1000);
    }

    SDLNet_FreePacket(sendPacket);
    SDLNet_FreePacket(recPacket);
    SDLNet_UDP_Close(sock);
    SDLNet_Quit();
    return 1;
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    initMusicSystem();
    playMusic("uclmusic.mp3");
    setMusicVolume(64);

    int screenWidth = 1280;
    int screenHeight = 720;

    SDL_Window *window = SDL_CreateWindow("Pong Prototyp",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          screenWidth, screenHeight,
                                          SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font *font = load_font("assets/assets2/DejaVuSans.ttf", 24);
    if (!font)
    {
        SDL_Quit();
        return 1;
    }

    char ipBuffer[16] = "";
    if (showClientMenu(renderer, font, ipBuffer) != 0)
    {
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 0;
    }

    if (!wait_for_all_players(ipBuffer)) {
        printf("Failed to connect to server or waiting failed.\n");
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    init_background(renderer, screenWidth, screenHeight);

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
                running = false;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        render_background(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    cleanup_background();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    stopMusic();

    return 0;
}
