#pragma once
#include <SDL2/SDL_mixer.h>

#define MAX_SOUNDS 256
void audioInit();
void audioQuit();

int loadSound(char* sampleName);
int playSound(int id);
void freeSound(int id);
void setVolume(unsigned char volume);
