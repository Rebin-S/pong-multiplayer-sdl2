#include "background.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

static SDL_Texture* bg_texture = NULL;
static SDL_Rect bg_dest;

void init_background(SDL_Renderer* renderer, int screenWidth, int screenHeight) {
    SDL_Surface* surface = IMG_Load("assets/assets2/bakgrundFotbollsplan.png");
    if (!surface) {
        SDL_Log("Failed to load background: %s", IMG_GetError());
        return;
    }

    bg_texture = SDL_CreateTextureFromSurface(renderer, surface);

    int imgWidth = surface->w;
    int imgHeight = surface->h;

    SDL_FreeSurface(surface);

    float scaleX = (float)screenWidth / imgWidth;
    float scaleY = (float)screenHeight / imgHeight;
    float scale = (scaleX > scaleY) ? scaleX : scaleY; 

    bg_dest.w = imgWidth * scale;
    bg_dest.h = imgHeight * scale;
    bg_dest.x = (screenWidth - bg_dest.w) / 2;
    bg_dest.y = (screenHeight - bg_dest.h) / 2;
}

void render_background(SDL_Renderer* renderer) {
    if (bg_texture) {
        SDL_RenderCopy(renderer, bg_texture, NULL, &bg_dest);
    }
}

void cleanup_background() {
    if (bg_texture) {
        SDL_DestroyTexture(bg_texture);
        bg_texture = NULL;
    }
}
