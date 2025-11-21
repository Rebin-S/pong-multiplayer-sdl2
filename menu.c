#include "menu.h"
#include "music.h"
#include <string.h>

int showServerMenu(SDL_Renderer *renderer, TTF_Font *font, int *mode, int *points)
{
   SDL_Surface *bgSurface = IMG_Load("assets/menyn.png");
    if (!bgSurface) {
        SDL_Log("Failed to load background in server menu: %s", IMG_GetError());
    }
    SDL_Texture *bgTexture = NULL;
    if (bgSurface) {
        bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
        SDL_FreeSurface(bgSurface);
    }

    int windowW, windowH;
    SDL_GetRendererOutputSize(renderer, &windowW, &windowH);

    SDL_Color white = {255, 255, 255};
    bool menuActive = true;
    bool optionsActive = true; 

    *mode = 1;
    *points = 1;

    SDL_Event e;

    while (menuActive)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                SDL_DestroyTexture(bgTexture);
                return 1;
            }

            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                SDL_Rect backRect = {windowW * 0.30, windowH * 0.8, windowW * 0.06, windowH * 0.04};
                SDL_Rect startRect = {windowW * 0.45, windowH * 0.72, windowW * 0.10, windowH * 0.05};

                if (optionsActive)
                {
                    SDL_Rect easyRect = {windowW * 0.35, windowH * 0.42, windowW * 0.06, windowH * 0.03};
                    SDL_Rect mediumRect = {windowW * 0.48, windowH * 0.41, windowW * 0.07, windowH * 0.04};
                    SDL_Rect hardRect = {windowW * 0.6, windowH * 0.42, windowW * 0.06, windowH * 0.03};
                    SDL_Rect sevenRect = {windowW * 0.42, windowH * 0.58, windowW * 0.03, windowH * 0.03};
                    SDL_Rect elevenRect = {windowW * 0.55, windowH * 0.58, windowW * 0.03, windowH * 0.03};

                    if (x >= easyRect.x && x <= easyRect.x + easyRect.w && y >= easyRect.y && y <= easyRect.y + easyRect.h) *mode = 1;
                    if (x >= mediumRect.x && x <= mediumRect.x + mediumRect.w && y >= mediumRect.y && y <= mediumRect.y + mediumRect.h) *mode = 2;
                    if (x >= hardRect.x && x <= hardRect.x + hardRect.w && y >= hardRect.y && y <= hardRect.y + hardRect.h) *mode = 3;
                    if (x >= sevenRect.x && x <= sevenRect.x + sevenRect.w && y >= sevenRect.y && y <= sevenRect.y + sevenRect.h) *points = 1;
                    if (x >= elevenRect.x && x <= elevenRect.x + elevenRect.w && y >= elevenRect.y && y <= elevenRect.y + elevenRect.h) *points = 2;
                    if (x >= backRect.x && x <= backRect.x + backRect.w && y >= backRect.y && y <= backRect.y + backRect.h) optionsActive = false;
                }
                else
                {
                    if (x >= startRect.x && x <= startRect.x + startRect.w && y >= startRect.y && y <= startRect.y + startRect.h)
                    {
                        SDL_DestroyTexture(bgTexture);
                        return 0;
                    }
                }
            }
        }

        SDL_RenderClear(renderer);
        if (bgTexture) {
            SDL_RenderCopy(renderer, bgTexture, NULL, NULL);
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 190);
        SDL_Rect overlayRect = {windowW * 0.25, windowH * 0.31, windowW * 0.50, windowH * 0.55};
        SDL_RenderFillRect(renderer, &overlayRect);

        if (optionsActive)
        {
            SDL_Surface *modeTextSurface = TTF_RenderText_Solid(font, "Difficulty Mode", white);
            SDL_Texture *modeText = SDL_CreateTextureFromSurface(renderer, modeTextSurface);
            SDL_FreeSurface(modeTextSurface);
            SDL_Rect modeRect = {windowW * 0.44, windowH * 0.33, windowW * 0.14, windowH * 0.06};
            SDL_RenderCopy(renderer, modeText, NULL, &modeRect);
            SDL_DestroyTexture(modeText);

            const char *diffs[] = {"easy", "medium", "hard"};
            SDL_Rect diffRects[] = {
                {windowW * 0.35, windowH * 0.42, windowW * 0.06, windowH * 0.03},
                {windowW * 0.48, windowH * 0.41, windowW * 0.07, windowH * 0.04},
                {windowW * 0.6, windowH * 0.42, windowW * 0.06, windowH * 0.03}};
            for (int i = 0; i < 3; i++) {
                SDL_Surface *surf = TTF_RenderText_Solid(font, diffs[i], white);
                SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_RenderCopy(renderer, tex, NULL, &diffRects[i]);
                SDL_FreeSurface(surf);
                SDL_DestroyTexture(tex);
            }
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 100);
            SDL_RenderFillRect(renderer, &diffRects[*mode - 1]);

            SDL_Surface *pointsTextSurface = TTF_RenderText_Solid(font, "Points", white);
            SDL_Texture *pointsText = SDL_CreateTextureFromSurface(renderer, pointsTextSurface);
            SDL_FreeSurface(pointsTextSurface);
            SDL_Rect pointsRect = {windowW * 0.46, windowH * 0.5, windowW * 0.10, windowH * 0.05};
            SDL_RenderCopy(renderer, pointsText, NULL, &pointsRect);
            SDL_DestroyTexture(pointsText);

            SDL_Rect sevenRect = {windowW * 0.42, windowH * 0.58, windowW * 0.03, windowH * 0.03};
            SDL_Rect elevenRect = {windowW * 0.55, windowH * 0.58, windowW * 0.03, windowH * 0.03};

            SDL_Surface *sevenSurface = TTF_RenderText_Solid(font, "7", white);
            SDL_Texture *sevenText = SDL_CreateTextureFromSurface(renderer, sevenSurface);
            SDL_FreeSurface(sevenSurface);
            SDL_RenderCopy(renderer, sevenText, NULL, &sevenRect);
            SDL_DestroyTexture(sevenText);

            SDL_Surface *elevenSurface = TTF_RenderText_Solid(font, "11", white);
            SDL_Texture *elevenText = SDL_CreateTextureFromSurface(renderer, elevenSurface);
            SDL_FreeSurface(elevenSurface);
            SDL_RenderCopy(renderer, elevenText, NULL, &elevenRect);
            SDL_DestroyTexture(elevenText);

            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 100);
            SDL_RenderFillRect(renderer, (*points == 1) ? &sevenRect : &elevenRect);

            SDL_Surface *backTextSurface = TTF_RenderText_Solid(font, "back", white);
            SDL_Texture *backText = SDL_CreateTextureFromSurface(renderer, backTextSurface);
            SDL_FreeSurface(backTextSurface);
            SDL_Rect backRect = {windowW * 0.30, windowH * 0.8, windowW * 0.06, windowH * 0.04};
            SDL_RenderCopy(renderer, backText, NULL, &backRect);
            SDL_DestroyTexture(backText);
        }
        else
        {
            SDL_Surface *startSurface = TTF_RenderText_Solid(font, "Start Game", white);
            SDL_Texture *startText = SDL_CreateTextureFromSurface(renderer, startSurface);
            SDL_FreeSurface(startSurface);
            SDL_Rect startRect = {windowW * 0.45, windowH * 0.72, windowW * 0.10, windowH * 0.05};
            SDL_RenderCopy(renderer, startText, NULL, &startRect);
            SDL_DestroyTexture(startText);
        }

        SDL_ShowCursor(SDL_TRUE);
        SDL_RenderPresent(renderer);
    }

     if (bgTexture) SDL_DestroyTexture(bgTexture);
    return 1;
}

int showClientMenu(SDL_Renderer *renderer, TTF_Font *font, char *ipBuffer)
{
    SDL_StartTextInput();
    int windowW, windowH;
    SDL_GetRendererOutputSize(renderer, &windowW, &windowH);

    SDL_Color white = {255, 255, 255};
    bool menuActive = true;
    bool optionsActive = false;

    int volume = 64;
    bool isMuted = false;
    int previousVolume = volume;

    SDL_Surface *bgSurface = IMG_Load("assets/menyn.png");
    SDL_Texture *bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);

    SDL_Event e;

    while (menuActive)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                return 1;

            if (!optionsActive && e.type == SDL_TEXTINPUT && strlen(ipBuffer) < 15)
                strcat(ipBuffer, e.text.text);

            if (!optionsActive && e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_BACKSPACE && strlen(ipBuffer) > 0)
                    ipBuffer[strlen(ipBuffer) - 1] = '\0';
                else if (e.key.keysym.sym == SDLK_RETURN && strlen(ipBuffer) > 0)
                    return 0;
            }

            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                if (!optionsActive)
                {
                    SDL_Rect optionsRect = {windowW * 0.35, windowH * 0.88, windowW * 0.10, windowH * 0.05};
                    if (x >= optionsRect.x && x <= optionsRect.x + optionsRect.w &&
                        y >= optionsRect.y && y <= optionsRect.y + optionsRect.h)
                    {
                        optionsActive = true;
                    }
                }
                else
                {
                    SDL_Rect backRect = {windowW * 0.30, windowH * 0.8, windowW * 0.06, windowH * 0.04};
                    if (x >= backRect.x && x <= backRect.x + backRect.w &&
                        y >= backRect.y && y <= backRect.y + backRect.h)
                    {
                        optionsActive = false;
                    }

                    SDL_Rect sliderRect = {windowW * 0.41, windowH * 0.75, windowW * 0.2, windowH * 0.01};
                    if (x >= sliderRect.x && x <= sliderRect.x + sliderRect.w &&
                        y >= sliderRect.y - 10 && y <= sliderRect.y + 20)
                    {
                        volume = (x - sliderRect.x) * 128 / sliderRect.w;
                        if (volume < 0) volume = 0;
                        if (volume > 128) volume = 128;
                        setMusicVolume(volume);
                    }

                    SDL_Rect muteRect = {windowW * 0.65, windowH * 0.65, windowW * 0.10, windowH * 0.04};
                    if (x >= muteRect.x && x <= muteRect.x + muteRect.w &&
                        y >= muteRect.y && y <= muteRect.y + muteRect.h)
                    {
                        if (isMuted) {
                            volume = previousVolume;
                            setMusicVolume(volume);
                            isMuted = false;
                        } else {
                            previousVolume = volume;
                            volume = 0;
                            setMusicVolume(0);
                            isMuted = true;
                        }
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

        if (!optionsActive)
        {
            SDL_Surface *labelSurface = TTF_RenderText_Solid(font, "Enter server IP:", white);
            SDL_Texture *labelText = SDL_CreateTextureFromSurface(renderer, labelSurface);
            SDL_Rect labelRect = {windowW / 2 - labelSurface->w / 2, windowH * 0.4, labelSurface->w, labelSurface->h};
            SDL_RenderCopy(renderer, labelText, NULL, &labelRect);
            SDL_FreeSurface(labelSurface);
            SDL_DestroyTexture(labelText);

            SDL_Surface *ipSurface = TTF_RenderText_Solid(font, ipBuffer[0] ? ipBuffer : "Type IP and press Enter", white);
            SDL_Texture *ipText = SDL_CreateTextureFromSurface(renderer, ipSurface);
            SDL_Rect ipRect = {windowW / 2 - 200, windowH * 0.48, 400, ipSurface->h};
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
            SDL_Rect bgBox = {ipRect.x - 10, ipRect.y - 5, ipRect.w + 20, ipRect.h + 10};
            SDL_RenderFillRect(renderer, &bgBox);
            SDL_RenderCopy(renderer, ipText, NULL, &ipRect);
            SDL_FreeSurface(ipSurface);
            SDL_DestroyTexture(ipText);

            SDL_Surface *optSurface = TTF_RenderText_Solid(font, "options", white);
            SDL_Texture *optText = SDL_CreateTextureFromSurface(renderer, optSurface);
            SDL_FreeSurface(optSurface);
            SDL_Rect optionsRect = {windowW * 0.35, windowH * 0.88, windowW * 0.10, windowH * 0.05};
            SDL_RenderCopy(renderer, optText, NULL, &optionsRect);
            SDL_DestroyTexture(optText);
        }
        else
        {
            SDL_Surface *volumeTextSurface = TTF_RenderText_Solid(font, "Volume", white);
            SDL_Texture *volumeText = SDL_CreateTextureFromSurface(renderer, volumeTextSurface);
            SDL_FreeSurface(volumeTextSurface);
            SDL_Rect volumeRect = {windowW * 0.46, windowH * 0.65, windowW * 0.10, windowH * 0.05};
            SDL_RenderCopy(renderer, volumeText, NULL, &volumeRect);
            SDL_DestroyTexture(volumeText);

            SDL_Rect sliderRect = {windowW * 0.41, windowH * 0.75, windowW * 0.2, windowH * 0.01};
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderFillRect(renderer, &sliderRect);
            int sliderX = sliderRect.x + (volume * sliderRect.w / 128) - 5;
            SDL_Rect sliderKnob = {sliderX, sliderRect.y - 5, 10, 20};
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &sliderKnob);

            SDL_Surface *muteSurface = TTF_RenderText_Solid(font, isMuted ? "Unmute" : "Mute", white);
            SDL_Texture *muteText = SDL_CreateTextureFromSurface(renderer, muteSurface);
            SDL_FreeSurface(muteSurface);
            SDL_Rect muteRect = {windowW * 0.65, windowH * 0.65, windowW * 0.10, windowH * 0.04};
            SDL_RenderCopy(renderer, muteText, NULL, &muteRect);
            SDL_DestroyTexture(muteText);

            SDL_Surface *backTextSurface = TTF_RenderText_Solid(font, "back", white);
            SDL_Texture *backText = SDL_CreateTextureFromSurface(renderer, backTextSurface);
            SDL_FreeSurface(backTextSurface);
            SDL_Rect backRect = {windowW * 0.30, windowH * 0.8, windowW * 0.06, windowH * 0.04};
            SDL_RenderCopy(renderer, backText, NULL, &backRect);
            SDL_DestroyTexture(backText);
        }

        SDL_ShowCursor(SDL_TRUE);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(bgTexture);
    return 1;
}