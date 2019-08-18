#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"

#pragma warning(disable:4996)

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c %c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')  

#define MEMORY_SIZE 0x10000 //16K

void setupState(State8080* state);
void readFileToMemory(State8080* state, char* filename, unsigned short offset);
void dumpMemory(State8080* state);

int main(int argc, char** argv)
{
	State8080* state = (State8080*)malloc(sizeof(State8080));
	int done = 0;

	setupState(state);
	readFileToMemory(state, "test.h", 0);
	
	readFileToMemory(state, "SpaceInvaders.h", 0);
	readFileToMemory(state, "SpaceInvaders.g", 0x800);
	readFileToMemory(state, "SpaceInvaders.f", 0x1000);
	readFileToMemory(state, "SpaceInvaders.e", 0x1800);
	dumpMemory(state);
	while (done == 0)
	{
		printf("\taBinaryBefore="BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(state->a));
		done = emulate8080Op(state);
		printf("\tpc=%d, sp=%d, a=%d, b=%d, c=%d, d=%d, e=%d, h=%d, l=%d, m=%d, bc=%d, de=%d, hl=%d\n", state->pc, state->sp, state->a, state->b, state->c, state->d, state->e, state->h, state->l, state->memory[((state->h << 8) | state->l)], (state->b << 8) | state->c, (state->d << 8) | state->e, (state->h << 8) | state->l);
		printf("\tz=%d, s=%d, p=%d, cy=%d, ac=%d\n", state->cc.z, state->cc.s, state->cc.p, state->cc.cy, state->cc.ac);
		printf("\taBinaryAfter="BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(state->a));
		system("PAUSE");
	}
		
	free(state->memory);
	free(state);
	
}

void setupState(State8080* state)
{
	state->a = 0;
	state->b = 0;
	state->c = 0;
	state->d = 0;
	state->e = 0;
	state->h = 0;
	state->l = 0;

	state->sp = 0xF000;
	state->pc = 0;

	state->memory = (byte*)malloc(MEMORY_SIZE);
	memset(state->memory, NULL, MEMORY_SIZE);

	state->cc.z = 1;
	state->cc.s = 0;
	state->cc.p = 1;
	state->cc.cy = 0;
	state->cc.ac = 1;
	state->cc.pad = 0;
}

void readFileToMemory(State8080* state, char* filename, unsigned short offset)
{
	FILE* f = fopen(filename, "rb");

	if (!f)
	{
		printf("Error: Couldn't open file %s!\n", filename);
		exit(1);
	}

	fseek(f, 0L, SEEK_END);
	int fsize = ftell(f);
	fseek(f, 0L, SEEK_SET);

	byte* buffer = &state->memory[offset];
	fread(buffer, fsize, 1, f);
	fclose(f);
}

void dumpMemory(State8080* state)
{
	FILE* f = fopen("log.txt", "w");

	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	int i = 0;
	for (i = 0; i < MEMORY_SIZE; ++i)
	{
		if (i % 16 == 0)
		{
			fprintf(f, "\n %d. ", i);
		}
		
		fprintf(f, "%3d ", state->memory[i]);
	}

	fclose(f);
}
