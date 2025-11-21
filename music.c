#include "music.h"
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

static Mix_Music *bgm = NULL;

void initMusicSystem() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }
}

void playMusic(const char *filename) {
    bgm = Mix_LoadMUS(filename);
    if (!bgm) {
        printf("Failed to load music: %s\n", Mix_GetError());
        return;
    }
    Mix_PlayMusic(bgm, -1); 
}

void setMusicVolume(int volume) {
    if (volume < 0) volume = 0;
    if (volume > 128) volume = 128;
    Mix_VolumeMusic(volume);
}

void stopMusic() {
    Mix_HaltMusic();
    if (bgm) {
        Mix_FreeMusic(bgm);
        bgm = NULL;
    }
    Mix_CloseAudio();
}
