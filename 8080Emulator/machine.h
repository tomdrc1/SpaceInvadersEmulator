#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cpu.h"
#include <SDL.h>

#define MEMORY_SIZE 0x10000 //16K

#define SCREEN_HEIGHT 256
#define SCREEN_WIDTH 224

#define CYCLES_PER_FRAME 2000000 / 60

typedef struct spaceInvaderMachine
{
	State8080* state;

	unsigned short xy;
	byte shift_offset;
	SDL_Window* screen;
	SDL_Renderer* renderer;
	SDL_Event sdlEvent;
	byte which_int;

	byte port1;
	byte port2;
} spaceInvaderMachine;

void startEmulation(spaceInvaderMachine* machine);
void setupState(State8080* state);
void setupMachine(spaceInvaderMachine* machine);
void readFileToMemory(State8080* state, char* filename, unsigned short offset);
void machineUpdate(spaceInvaderMachine* machine);
byte machineIn(spaceInvaderMachine* machine, byte port);
void machineOut(spaceInvaderMachine* machine, byte port, byte value);
void draw(spaceInvaderMachine* state);
int Disassemble8080Op(unsigned char* codebuffer, int pc, FILE* f);