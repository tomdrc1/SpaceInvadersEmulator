#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cpu.h"
#include <SDL.h>

#define MEMORY_SIZE 0x10000 //16K

typedef struct spaceInvaderMachine
{
	State8080* state;

	unsigned short xy;
	byte shift_offset;
	SDL_Window* screen;
	SDL_Renderer* renderer;
	SDL_Event sdlEvent;
} spaceInvaderMachine;

void startEmulation(spaceInvaderMachine* machine);
void setupState(State8080* state);
void readFileToMemory(State8080* state, char* filename, unsigned short offset);
int Disassemble8080Op(unsigned char* codebuffer, int pc, FILE* f);
byte machineIn(spaceInvaderMachine* machine, byte port);
void machineOut(spaceInvaderMachine* machine, byte port, byte value);
void draw(spaceInvaderMachine* state);