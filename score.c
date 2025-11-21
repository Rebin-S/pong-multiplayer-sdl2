#include "score.h"
#include <stdio.h>

TTF_Font *load_font(const char *path, int size)
{
    printf("forsoker ladda font från: %s\n", path);

    TTF_Font *font = TTF_OpenFont(path, size);
    if (!font)
    {
        printf("Kunde inte ladda font: %s\n", TTF_GetError());
        return NULL;
    }
    return font;
}

void draw_scores(SDL_Renderer *renderer, TTF_Font *font, int team1_score, int team2_score, int screenWidth)
{
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color blue = {0, 0, 255, 255};

    char text1[10], text2[10];
    sprintf(text1, "%d", team1_score);
    sprintf(text2, "%d", team2_score);

    SDL_Surface *surface1 = TTF_RenderText_Solid(font, text1, red);
    SDL_Surface *surface2 = TTF_RenderText_Solid(font, text2, blue);

    SDL_Texture *texture1 = SDL_CreateTextureFromSurface(renderer, surface1);
    SDL_Texture *texture2 = SDL_CreateTextureFromSurface(renderer, surface2);

    SDL_Rect rect1 = {50, 50, surface1->w, surface1->h};
    SDL_Rect rect2 = {screenWidth - 50 - surface2->w, 50, surface2->w, surface2->h};

    SDL_FreeSurface(surface1);
    SDL_FreeSurface(surface2);

    SDL_RenderCopy(renderer, texture1, NULL, &rect1);
    SDL_RenderCopy(renderer, texture2, NULL, &rect2);

    SDL_DestroyTexture(texture1);
    SDL_DestroyTexture(texture2);
}
void show_game_over(SDL_Renderer *renderer, TTF_Font *font, int winner)
{
    SDL_Color color;
    const char *message;

    if (winner == 1)
    {
        color = (SDL_Color){255, 0, 0, 255}; 
        message = "Team Red wins!";
    }
    else
    {
        color = (SDL_Color){0, 0, 255, 255}; 
        message = "Team Blue wins!";
    }

    SDL_Surface *surface = TTF_RenderText_Solid(font, message, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

    SDL_Rect rect = {
        screenWidth / 2 - surface->w / 2,
        screenHeight / 2 - surface->h / 2,
        surface->w,
        surface->h};

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_RenderPresent(renderer);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_Delay(3000); 
}

Score init_score()
{
    Score s = {0, 0};
    return s;
}
