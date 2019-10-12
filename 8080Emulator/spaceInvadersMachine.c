#include "spaceInvadersMachine.h"

void startEmulation(spaceInvaderMachine* machine)
{
	machine->state = (State8080*)malloc(sizeof(State8080));

	int done = 0;
	initMachine(machine);

	readFileToMemory(machine->state, "Game\\SpaceInvaders.h", 0);
	readFileToMemory(machine->state, "Game\\SpaceInvaders.g", 0x800);
	readFileToMemory(machine->state, "Game\\SpaceInvaders.f", 0x1000);
	readFileToMemory(machine->state, "Game\\SpaceInvaders.e", 0x1800);

	unsigned int timer = SDL_GetTicks();
	
	while (!done)
	{
		if (SDL_PollEvent(&machine->sdlEvent) != 0)
		{
			if (machine->sdlEvent.type == SDL_QUIT)
			{
				done = 1;
			}
			else if (machine->sdlEvent.type == SDL_KEYDOWN)
			{
				const unsigned int key = machine->sdlEvent.key.keysym.scancode;
				if (key == SDL_SCANCODE_C)
				{
					machine->port1 |= 1 << 0; // coin
				}
				else if (key == SDL_SCANCODE_2)
				{
					machine->port1 |= 1 << 1; // P2 start button
				}
				else if (key == SDL_SCANCODE_RETURN) 
				{
					machine->port1 |= 1 << 2; // P1 start button
				}
				else if (key == SDL_SCANCODE_SPACE)
				{
					machine->port1 |= 1 << 4; // P1 shoot button
					machine->port2 |= 1 << 4; // P2 shoot button
				}
				else if (key == SDL_SCANCODE_LEFT) 
				{
					machine->port1 |= 1 << 5; // P1 joystick left
					machine->port2 |= 1 << 5; // P2 joystick left
				}
				else if (key == SDL_SCANCODE_RIGHT)
				{
					machine->port1 |= 1 << 6; // P1 joystick right
					machine->port2 |= 1 << 6; // P2 joystick right
				}
				else if (key == SDL_SCANCODE_T) 
				{
					machine->port2 |= 1 << 2; // tilt
				}
			}
			else if (machine->sdlEvent.type == SDL_KEYUP)
			{
				const unsigned int key = machine->sdlEvent.key.keysym.scancode;

				if (key == SDL_SCANCODE_C)
				{
					machine->port1 &= 0b11111110; // coin
				}
				else if (key == SDL_SCANCODE_2)
				{
					machine->port1 &= 0b11111101; // P2 start button
				}
				else if (key == SDL_SCANCODE_RETURN)
				{
					machine->port1 &= 0b11111011; // P1 start button
				}
				else if (key == SDL_SCANCODE_SPACE)
				{
					machine->port1 &= 0b11101111; // P1 shoot button
					machine->port2 &= 0b11101111; // P2 shoot button
				}
				else if (key == SDL_SCANCODE_LEFT) 
				{
					machine->port1 &= 0b11011111; // P1 joystick left
					machine->port2 &= 0b11011111; // P2 joystick left
				}
				else if (key == SDL_SCANCODE_RIGHT)
				{
					machine->port1 &= 0b10111111; // P1 joystick right
					machine->port2 &= 0b10111111; // P2 joystick right
				}
				else if (key == SDL_SCANCODE_T) 
				{
					machine->port2 &= 0b11111011; // tilt
				}
			}
			else if (machine->sdlEvent.type == SDL_WINDOWEVENT && machine->sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				SDL_RenderSetScale(machine->renderer, (float)machine->sdlEvent.window.data1 / SCREEN_WIDTH, (float)machine->sdlEvent.window.data2 / SCREEN_HEIGHT);
			}
		}

		if (SDL_GetTicks() - timer > (1.f / 60) * 1000)
		{
			timer = SDL_GetTicks();
			machineUpdate(machine);
			draw(machine);
		}
	}

	audioQuit();
	free(machine->state->memory);
	free(machine->state);
}

void initState(State8080* state)
{
	state->a = 0;
	state->b = 0;
	state->c = 0;
	state->d = 0;
	state->e = 0;
	state->h = 0;
	state->l = 0;

	state->sp = 0x2400;
	state->pc = 0;

	state->memory = (byte*)malloc(MEMORY_SIZE);
	memset(state->memory, NULL, MEMORY_SIZE);

	state->cc.z = 0;
	state->cc.s = 0;
	state->cc.p = 0;
	state->cc.cy = 0;
	state->cc.ac = 0;
	state->cc.pad1 = 1;
	state->cc.pad2 = 0;
	state->cc.pad3 = 0;
	state->int_enable = 0;
	state->cycles = 0;
}

void initMachine(spaceInvaderMachine* machine)
{
	audioInit();
	initState(machine->state);
	machine->shiftOffset = 0;
	machine->xy = 0;
	machine->whichInt = 0x01;

	machine->port1 = 1 << 3;
	machine->port2 = 0;

	machine->soundsId[0] = loadSound("Sounds\\8.wav"); // ufo sound
	machine->soundsId[1] = loadSound("Sounds\\1.wav"); // shoot sound
	machine->soundsId[2] = loadSound("Sounds\\2.wav"); // player die
	machine->soundsId[3] = loadSound("Sounds\\3.wav"); // alien die
	machine->soundsId[4] = loadSound("Sounds\\4.wav"); // alien move 1
	machine->soundsId[5] = loadSound("Sounds\\5.wav"); // alien move 2
	machine->soundsId[6] = loadSound("Sounds\\6.wav"); // alien move 3
	machine->soundsId[7] = loadSound("Sounds\\7.wav"); // alien move 4
	machine->soundsId[8] = loadSound("Sounds\\10.wav"); // ufo hit

	setVolume(15);
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

void machineUpdate(spaceInvaderMachine* machine)
{
	unsigned int cyclesCount = 0;

	while (cyclesCount <= CYCLES_PER_FRAME)
	{
		unsigned int startCycle = machine->state->cycles;
		byte* instruction = &machine->state->memory[machine->state->pc];
		
		emulate8080Op(machine->state);
		cyclesCount += machine->state->cycles - startCycle;

		if (*instruction == 0xdb) //machine specific handling for IN    
		{
			byte port = instruction[1];
			machine->state->a = machineIn(machine, port);
			machine->state->pc++;
		}
		else if (*instruction == 0xd3)  //OUT    
		{
			byte port = instruction[1];
			machineOut(machine, port, machine->state->a);
			machine->state->pc++;
		}

		if (machine->state->cycles >= (CYCLES_PER_FRAME / 2))
		{
			generateInterrupt(machine->state, machine->whichInt);
			machine->state->cycles -= (CYCLES_PER_FRAME / 2);
			machine->whichInt = (machine->whichInt == 0x01 ? 0x02 : 0x01);
		}
	}
}

byte machineIn(spaceInvaderMachine* machine, byte port)
{
	switch (port)
	{
		case 1:
			return machine->port1;
		case 2:
			return machine->port2;
		case 3:
			{
				return ((machine->xy >> (8 - machine->shiftOffset)) & 0xFF);
			}
			break;
	}
}

void machineOut(spaceInvaderMachine* machine, byte port, byte value)
{
	switch (port)
	{
		case 2:
			machine->shiftOffset = (value & 0x07);
			break;
		case 3:
			playSounds(machine, port, machine->state->a);
			break;
		case 4:
			machine->xy >>= 8;
			machine->xy |= (value << 8);
			break;
		case 5:
			playSounds(machine, port, machine->state->a);
			break;
	}
}

void playSounds(spaceInvaderMachine* machine, byte port, byte value)
{
	switch (port)
	{
		case 3:
			if (value != machine->lastOutPort3)
			{
				if ((value & 0x01) && !(machine->lastOutPort3 & 0x01))
				{
					playSound(machine->soundsId[0]);
				}
				if ((value & 0x02) && !(machine->lastOutPort3 & 0x02))
				{
					playSound(machine->soundsId[1]);
				}
				if ((value & 0x04) && !(machine->lastOutPort3 & 0x04))
				{
					playSound(machine->soundsId[2]);
				}
				if ((value & 0x08) && !(machine->lastOutPort3 & 0x08))
				{
					playSound(machine->soundsId[3]);
				}
				
				machine->lastOutPort3 = value;
			}
			break;
		case 5:
			if (value != machine->lastOutPort5)
			{
				if ((value & 0x01) && !(machine->lastOutPort5 & 0x01))
				{
					playSound(machine->soundsId[4]);
				}
				if ((value & 0x02) && !(machine->lastOutPort5 & 0x02))
				{
					playSound(machine->soundsId[5]);
				}
				if ((value & 0x04) && !(machine->lastOutPort5 & 0x04))
				{
					playSound(machine->soundsId[6]);
				}
				if ((value & 0x08) && !(machine->lastOutPort5 & 0x08))
				{
					playSound(machine->soundsId[7]);
				}
				if ((value & 0x10) && !(machine->lastOutPort5 & 0x10))
				{
					playSound(machine->soundsId[8]);
				}
				machine->lastOutPort5 = value;
			}
			break;
	}
}

/*
	This function will draw the current sprites that are in the memory to the screen
	Input: A pointer to a struct that represents the spcaeInvadersMachine

 the screen is 256 * 224 pixels, and is rotated anti-clockwise.
 these are the overlay dimensions:
 ,_______________________________.
 |WHITE            ^             |
 |                32             |
 |                 v             |
 |-------------------------------|
 |RED              ^             |
 |                32             |
 |                 v             |
 |-------------------------------|
 |WHITE                          |
 |         < 224 >               |
 |                               |
 |                 ^             |
 |                120            |
 |                 v             |
 |                               |
 |                               |
 |                               |
 |-------------------------------|
 |GREEN                          |
 | ^                  ^          |
 |56        ^        56          |
 | v       72         v          |
 |____      v      ______________|
 |  ^  |          | ^            |
 |<16> |  < 118 > |16   < 122 >  |
 |  v  |          | v            |
 |WHITE|          |         WHITE|
 `-------------------------------'
 Beacuse the sapce invaders is rotated, we act like the x is our Y when in reality the screen in just rotated 90 degress to the right
*/
void draw(spaceInvaderMachine* machine)
{
	int i = 0;
	
	SDL_SetRenderDrawColor(machine->renderer, 0, 0, 0, 0);
	SDL_RenderClear(machine->renderer);
	SDL_SetRenderDrawColor(machine->renderer, 255, 255, 255, 255);

	for (i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH / 8; i++)
	{
		byte currentByte = machine->state->memory[VRAM_START + i];

		int p = 0;
		for (p = 7; p >= 0; --p)
		{
			if ((currentByte >> p) & 0x01)
			{
				unsigned short index = (i * 8) + p;
				byte x = index % SCREEN_HEIGHT;
				byte y = index / SCREEN_HEIGHT;

				//red
				if (x >= 192 && x < 224)
				{
					SDL_SetRenderDrawColor(machine->renderer, 255, 0, 0, 0);
				}
				//Green
				else if ((x > 16 && x <= 72) || (x < 16 && y >= 16 && y <= 134))
				{
					SDL_SetRenderDrawColor(machine->renderer, 0, 255, 0, 0);
				}
				else
				{
					SDL_SetRenderDrawColor(machine->renderer, 255, 255, 255, 255);
				}
				SDL_RenderDrawPoint(machine->renderer, y, SCREEN_HEIGHT - x);
			}
		}
	}
	
	SDL_RenderPresent(machine->renderer);
}