#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <wiringPi.h>
#include "cpu.h"
#include "audio.h"

#define MEMORY_SIZE 0x10000 //16K
#define VRAM_START 0x2400

#define SCREEN_HEIGHT 256
#define SCREEN_WIDTH 224

#define CYCLES_PER_FRAME 2000000 / 60

#define PLAYER_SHOT_PIN 15
#define PLAYER_LEFT_PIN 4
#define PLAYER_RIGHT_PIN 3
#define START_2PLAYER_PIN 5
#define PLAYER_START_PIN 12
#define COIN_INSERT_PIN 13

typedef struct spaceInvaderMachine
{
	SDL_Window* screen;
	SDL_Renderer* renderer;
	SDL_Event sdlEvent;

	State8080* state;
	unsigned short xy;
	byte shiftOffset;
	byte port1;
	byte port2;
	byte whichInt;
	byte lastOutPort3;
	byte lastOutPort5;
	int soundsId[9];
} spaceInvaderMachine;

void startEmulation(spaceInvaderMachine* machine);
void initState(State8080* state);
void initMachine(spaceInvaderMachine* machine);
void initPiPins();
void readFileToMemory(State8080* state, char* filename, unsigned short offset);
void machineUpdate(spaceInvaderMachine* machine);
byte machineIn(spaceInvaderMachine* machine, byte port);
void machineOut(spaceInvaderMachine* machine, byte port, byte value);
void playSounds(spaceInvaderMachine* machine, byte port, byte value);
void draw(spaceInvaderMachine* state);
