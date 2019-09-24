#include "spaceInvadersMachine.h"

void startEmulation(spaceInvaderMachine* machine)
{
	machine->state = (State8080*)malloc(sizeof(State8080));

	int done = 0;
	initMachine(machine);

	readFileToMemory(machine->state, "./Game/SpaceInvaders.h", 0);
	readFileToMemory(machine->state, "./Game/SpaceInvaders.g", 0x800);
	readFileToMemory(machine->state, "./Game/SpaceInvaders.f", 0x1000);
	readFileToMemory(machine->state, "./Game/SpaceInvaders.e", 0x1800);

	unsigned int timer = SDL_GetTicks();
	
	while (!done)
	{

		if (digitalRead(7) == 0) //We do it reverse
		{
			machine->port1 |= 1 << 4; // P1 shoot button
			machine->port2 |= 1 << 4; // P2 shoot button
		}
		else if (digitalRead(7) == 1)
		{
			machine->port1 &= 0b11101111; // P1 shoot button
			machine->port2 &= 0b11101111; // P2 shoot button
		}
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
//				else if (key == SDL_SCANCODE_SPACE )
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

	machine->soundsId[0] = loadSound("./Sounds/8.wav"); // ufo sound
	machine->soundsId[1] = loadSound("./Sounds/1.wav"); // shoot sound
	machine->soundsId[2] = loadSound("./Sounds/2.wav"); // player die
	machine->soundsId[3] = loadSound("./Sounds/3.wav"); // alien die
	machine->soundsId[4] = loadSound("./Sounds/4.wav"); // alien move 1
	machine->soundsId[5] = loadSound("./Sounds/5.wav"); // alien move 2
	machine->soundsId[6] = loadSound("./Sounds/6.wav"); // alien move 3
	machine->soundsId[7] = loadSound("./Sounds/7.wav"); // alien move 4
	machine->soundsId[8] = loadSound("./Sounds/10.wav"); // ufo hit

	setVolume(15);
}

/*
	This function will initiate the pins
*/
void initPiPins()
{

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
				SDL_RenderDrawPoint(machine->renderer, y, (SCREEN_HEIGHT - x));
			}
		}
	}
	
	SDL_RenderPresent(machine->renderer);
}

int Disassemble8080Op(unsigned char* codebuffer, int pc, FILE* f)
{
	unsigned char* code = &codebuffer[pc];
	int opbytes = 1;
	fprintf(f, "%04x ", pc);
	switch (*code)
	{
	case 0x00: fprintf(f, "NOP\n"); break;
	case 0x01: fprintf(f, "LXI    B,#$%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0x02: fprintf(f, "STAX   B\n"); break;
	case 0x03: fprintf(f, "INX    B\n"); break;
	case 0x04: fprintf(f, "INR    B\n"); break;
	case 0x05: fprintf(f, "DCR    B\n"); break;
	case 0x06: fprintf(f, "MVI    B,#$%02x\n", code[1]); opbytes = 2; break;
	case 0x07: fprintf(f, "RLC\n"); break;
	case 0x08: fprintf(f, "NOP\n"); break;
	case 0x09: fprintf(f, "DAD    B\n"); break;
	case 0x0a: fprintf(f, "LDAX   B\n"); break;
	case 0x0b: fprintf(f, "DCX    B\n"); break;
	case 0x0c: fprintf(f, "INR    C\n"); break;
	case 0x0d: fprintf(f, "DCR    C\n"); break;
	case 0x0e: fprintf(f, "MVI    C,#$%02x\n", code[1]); opbytes = 2;	break;
	case 0x0f: fprintf(f, "RRC\n"); break;

	case 0x10: fprintf(f, "NOP\n"); break;
	case 0x11: fprintf(f, "LXI    D,#$%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0x12: fprintf(f, "STAX   D\n"); break;
	case 0x13: fprintf(f, "INX    D\n"); break;
	case 0x14: fprintf(f, "INR    D\n"); break;
	case 0x15: fprintf(f, "DCR    D\n"); break;
	case 0x16: fprintf(f, "MVI    D,#$%02x\n", code[1]); opbytes = 2; break;
	case 0x17: fprintf(f, "RAL\n"); break;
	case 0x18: fprintf(f, "NOP\n"); break;
	case 0x19: fprintf(f, "DAD    D\n"); break;
	case 0x1a: fprintf(f, "LDAX   D\n"); break;
	case 0x1b: fprintf(f, "DCX    D\n"); break;
	case 0x1c: fprintf(f, "INR    E\n"); break;
	case 0x1d: fprintf(f, "DCR    E\n"); break;
	case 0x1e: fprintf(f, "MVI    E,#$%02x\n", code[1]); opbytes = 2; break;
	case 0x1f: fprintf(f, "RAR\n"); break;

	case 0x20: fprintf(f, "NOP\n"); break;
	case 0x21: fprintf(f, "LXI    H,#$%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0x22: fprintf(f, "SHLD   $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0x23: fprintf(f, "INX    H\n"); break;
	case 0x24: fprintf(f, "INR    H\n"); break;
	case 0x25: fprintf(f, "DCR    H\n"); break;
	case 0x26: fprintf(f, "MVI    H,#$%02x\n", code[1]); opbytes = 2; break;
	case 0x27: fprintf(f, "DAA\n"); break;
	case 0x28: fprintf(f, "NOP\n"); break;
	case 0x29: fprintf(f, "DAD    H\n"); break;
	case 0x2a: fprintf(f, "LHLD   $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0x2b: fprintf(f, "DCX    H\n"); break;
	case 0x2c: fprintf(f, "INR    L\n"); break;
	case 0x2d: fprintf(f, "DCR    L\n"); break;
	case 0x2e: fprintf(f, "MVI    L,#$%02x\n", code[1]); opbytes = 2; break;
	case 0x2f: fprintf(f, "CMA\n"); break;

	case 0x30: fprintf(f, "NOP\n"); break;
	case 0x31: fprintf(f, "LXI    SP,#$%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0x32: fprintf(f, "STA    $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0x33: fprintf(f, "INX    SP\n"); break;
	case 0x34: fprintf(f, "INR    M\n"); break;
	case 0x35: fprintf(f, "DCR    M\n"); break;
	case 0x36: fprintf(f, "MVI    M,#$%02x\n", code[1]); opbytes = 2; break;
	case 0x37: fprintf(f, "STC\n"); break;
	case 0x38: fprintf(f, "NOP\n"); break;
	case 0x39: fprintf(f, "DAD    SP\n"); break;
	case 0x3a: fprintf(f, "LDA    $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0x3b: fprintf(f, "DCX    SP\n"); break;
	case 0x3c: fprintf(f, "INR    A\n"); break;
	case 0x3d: fprintf(f, "DCR    A\n"); break;
	case 0x3e: fprintf(f, "MVI    A,#$%02x\n", code[1]); opbytes = 2; break;
	case 0x3f: fprintf(f, "CMC\n"); break;

	case 0x40: fprintf(f, "MOV    B,B\n"); break;
	case 0x41: fprintf(f, "MOV    B,C\n"); break;
	case 0x42: fprintf(f, "MOV    B,D\n"); break;
	case 0x43: fprintf(f, "MOV    B,E\n"); break;
	case 0x44: fprintf(f, "MOV    B,H\n"); break;
	case 0x45: fprintf(f, "MOV    B,L\n"); break;
	case 0x46: fprintf(f, "MOV    B,M\n"); break;
	case 0x47: fprintf(f, "MOV    B,A\n"); break;
	case 0x48: fprintf(f, "MOV    C,B\n"); break;
	case 0x49: fprintf(f, "MOV    C,C\n"); break;
	case 0x4a: fprintf(f, "MOV    C,D\n"); break;
	case 0x4b: fprintf(f, "MOV    C,E\n"); break;
	case 0x4c: fprintf(f, "MOV    C,H\n"); break;
	case 0x4d: fprintf(f, "MOV    C,L\n"); break;
	case 0x4e: fprintf(f, "MOV    C,M\n"); break;
	case 0x4f: fprintf(f, "MOV    C,A\n"); break;

	case 0x50: fprintf(f, "MOV    D,B\n"); break;
	case 0x51: fprintf(f, "MOV    D,C\n"); break;
	case 0x52: fprintf(f, "MOV    D,D\n"); break;
	case 0x53: fprintf(f, "MOV    D.E\n"); break;
	case 0x54: fprintf(f, "MOV    D,H\n"); break;
	case 0x55: fprintf(f, "MOV    D,L\n"); break;
	case 0x56: fprintf(f, "MOV    D,M\n"); break;
	case 0x57: fprintf(f, "MOV    D,A\n"); break;
	case 0x58: fprintf(f, "MOV    E,B\n"); break;
	case 0x59: fprintf(f, "MOV    E,C\n"); break;
	case 0x5a: fprintf(f, "MOV    E,D\n"); break;
	case 0x5b: fprintf(f, "MOV    E,E\n"); break;
	case 0x5c: fprintf(f, "MOV    E,H\n"); break;
	case 0x5d: fprintf(f, "MOV    E,L\n"); break;
	case 0x5e: fprintf(f, "MOV    E,M\n"); break;
	case 0x5f: fprintf(f, "MOV    E,A\n"); break;

	case 0x60: fprintf(f, "MOV    H,B\n"); break;
	case 0x61: fprintf(f, "MOV    H,C\n"); break;
	case 0x62: fprintf(f, "MOV    H,D\n"); break;
	case 0x63: fprintf(f, "MOV    H.E\n"); break;
	case 0x64: fprintf(f, "MOV    H,H\n"); break;
	case 0x65: fprintf(f, "MOV    H,L\n"); break;
	case 0x66: fprintf(f, "MOV    H,M\n"); break;
	case 0x67: fprintf(f, "MOV    H,A\n"); break;
	case 0x68: fprintf(f, "MOV    L,B\n"); break;
	case 0x69: fprintf(f, "MOV    L,C\n"); break;
	case 0x6a: fprintf(f, "MOV    L,D\n"); break;
	case 0x6b: fprintf(f, "MOV    L,E\n"); break;
	case 0x6c: fprintf(f, "MOV    L,H\n"); break;
	case 0x6d: fprintf(f, "MOV    L,L\n"); break;
	case 0x6e: fprintf(f, "MOV    L,M\n"); break;
	case 0x6f: fprintf(f, "MOV    L,A\n"); break;

	case 0x70: fprintf(f, "MOV    M,B\n"); break;
	case 0x71: fprintf(f, "MOV    M,C\n"); break;
	case 0x72: fprintf(f, "MOV    M,D\n"); break;
	case 0x73: fprintf(f, "MOV    M.E\n"); break;
	case 0x74: fprintf(f, "MOV    M,H\n"); break;
	case 0x75: fprintf(f, "MOV    M,L\n"); break;
	case 0x76: fprintf(f, "HLT\n");        break;
	case 0x77: fprintf(f, "MOV    M,A\n"); break;
	case 0x78: fprintf(f, "MOV    A,B\n"); break;
	case 0x79: fprintf(f, "MOV    A,C\n"); break;
	case 0x7a: fprintf(f, "MOV    A,D\n"); break;
	case 0x7b: fprintf(f, "MOV    A,E\n"); break;
	case 0x7c: fprintf(f, "MOV    A,H\n"); break;
	case 0x7d: fprintf(f, "MOV    A,L\n"); break;
	case 0x7e: fprintf(f, "MOV    A,M\n"); break;
	case 0x7f: fprintf(f, "MOV    A,A\n"); break;

	case 0x80: fprintf(f, "ADD    B\n"); break;
	case 0x81: fprintf(f, "ADD    C\n"); break;
	case 0x82: fprintf(f, "ADD    D\n"); break;
	case 0x83: fprintf(f, "ADD    E\n"); break;
	case 0x84: fprintf(f, "ADD    H\n"); break;
	case 0x85: fprintf(f, "ADD    L\n"); break;
	case 0x86: fprintf(f, "ADD    M\n"); break;
	case 0x87: fprintf(f, "ADD    A\n"); break;
	case 0x88: fprintf(f, "ADC    B\n"); break;
	case 0x89: fprintf(f, "ADC    C\n"); break;
	case 0x8a: fprintf(f, "ADC    D\n"); break;
	case 0x8b: fprintf(f, "ADC    E\n"); break;
	case 0x8c: fprintf(f, "ADC    H\n"); break;
	case 0x8d: fprintf(f, "ADC    L\n"); break;
	case 0x8e: fprintf(f, "ADC    M\n"); break;
	case 0x8f: fprintf(f, "ADC    A\n"); break;

	case 0x90: fprintf(f, "SUB    B\n"); break;
	case 0x91: fprintf(f, "SUB    C\n"); break;
	case 0x92: fprintf(f, "SUB    D\n"); break;
	case 0x93: fprintf(f, "SUB    E\n"); break;
	case 0x94: fprintf(f, "SUB    H\n"); break;
	case 0x95: fprintf(f, "SUB    L\n"); break;
	case 0x96: fprintf(f, "SUB    M\n"); break;
	case 0x97: fprintf(f, "SUB    A\n"); break;
	case 0x98: fprintf(f, "SBB    B\n"); break;
	case 0x99: fprintf(f, "SBB    C\n"); break;
	case 0x9a: fprintf(f, "SBB    D\n"); break;
	case 0x9b: fprintf(f, "SBB    E\n"); break;
	case 0x9c: fprintf(f, "SBB    H\n"); break;
	case 0x9d: fprintf(f, "SBB    L\n"); break;
	case 0x9e: fprintf(f, "SBB    M\n"); break;
	case 0x9f: fprintf(f, "SBB    A\n"); break;

	case 0xa0: fprintf(f, "ANA    B\n"); break;
	case 0xa1: fprintf(f, "ANA    C\n"); break;
	case 0xa2: fprintf(f, "ANA    D\n"); break;
	case 0xa3: fprintf(f, "ANA    E\n"); break;
	case 0xa4: fprintf(f, "ANA    H\n"); break;
	case 0xa5: fprintf(f, "ANA    L\n"); break;
	case 0xa6: fprintf(f, "ANA    M\n"); break;
	case 0xa7: fprintf(f, "ANA    A\n"); break;
	case 0xa8: fprintf(f, "XRA    B\n"); break;
	case 0xa9: fprintf(f, "XRA    C\n"); break;
	case 0xaa: fprintf(f, "XRA    D\n"); break;
	case 0xab: fprintf(f, "XRA    E\n"); break;
	case 0xac: fprintf(f, "XRA    H\n"); break;
	case 0xad: fprintf(f, "XRA    L\n"); break;
	case 0xae: fprintf(f, "XRA    M\n"); break;
	case 0xaf: fprintf(f, "XRA    A\n"); break;

	case 0xb0: fprintf(f, "ORA    B\n"); break;
	case 0xb1: fprintf(f, "ORA    C\n"); break;
	case 0xb2: fprintf(f, "ORA    D\n"); break;
	case 0xb3: fprintf(f, "ORA    E\n"); break;
	case 0xb4: fprintf(f, "ORA    H\n"); break;
	case 0xb5: fprintf(f, "ORA    L\n"); break;
	case 0xb6: fprintf(f, "ORA    M\n"); break;
	case 0xb7: fprintf(f, "ORA    A\n"); break;
	case 0xb8: fprintf(f, "CMP    B\n"); break;
	case 0xb9: fprintf(f, "CMP    C\n"); break;
	case 0xba: fprintf(f, "CMP    D\n"); break;
	case 0xbb: fprintf(f, "CMP    E\n"); break;
	case 0xbc: fprintf(f, "CMP    H\n"); break;
	case 0xbd: fprintf(f, "CMP    L\n"); break;
	case 0xbe: fprintf(f, "CMP    M\n"); break;
	case 0xbf: fprintf(f, "CMP    A\n"); break;

	case 0xc0: fprintf(f, "RNZ\n"); break;
	case 0xc1: fprintf(f, "POP    B\n"); break;
	case 0xc2: fprintf(f, "JNZ    $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xc3: fprintf(f, "JMP    $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xc4: fprintf(f, "CNZ    $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xc5: fprintf(f, "PUSH   B\n"); break;
	case 0xc6: fprintf(f, "ADI    #$%02x\n", code[1]); opbytes = 2; break;
	case 0xc7: fprintf(f, "RST    0\n"); break;
	case 0xc8: fprintf(f, "RZ\n"); break;
	case 0xc9: fprintf(f, "RET\n"); break;
	case 0xca: fprintf(f, "JZ     $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xcb: fprintf(f, "JMP    $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xcc: fprintf(f, "CZ     $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xcd: fprintf(f, "CALL   $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xce: fprintf(f, "ACI    #$%02x\n", code[1]); opbytes = 2; break;
	case 0xcf: fprintf(f, "RST    1\n"); break;

	case 0xd0: fprintf(f, "RNC\n"); break;
	case 0xd1: fprintf(f, "POP    D\n"); break;
	case 0xd2: fprintf(f, "JNC    $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xd3: fprintf(f, "OUT    #$%02x\n", code[1]); opbytes = 2; break;
	case 0xd4: fprintf(f, "CNC    $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xd5: fprintf(f, "PUSH   D\n"); break;
	case 0xd6: fprintf(f, "SUI    #$%02x\n", code[1]); opbytes = 2; break;
	case 0xd7: fprintf(f, "RST    2\n"); break;
	case 0xd8: fprintf(f, "RC\n");  break;
	case 0xd9: fprintf(f, "RET\n"); break;
	case 0xda: fprintf(f, "JC     $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xdb: fprintf(f, "IN     #$%02x\n", code[1]); opbytes = 2; break;
	case 0xdc: fprintf(f, "CC     $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xdd: fprintf(f, "CALL   $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xde: fprintf(f, "SBI    #$%02x\n", code[1]); opbytes = 2; break;
	case 0xdf: fprintf(f, "RST    3\n"); break;

	case 0xe0: fprintf(f, "RPO\n"); break;
	case 0xe1: fprintf(f, "POP    H\n"); break;
	case 0xe2: fprintf(f, "JPO    $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xe3: fprintf(f, "XTHL\n"); break;
	case 0xe4: fprintf(f, "CPO    $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xe5: fprintf(f, "PUSH   H\n"); break;
	case 0xe6: fprintf(f, "ANI    #$%02x\n", code[1]); opbytes = 2; break;
	case 0xe7: fprintf(f, "RST    4\n"); break;
	case 0xe8: fprintf(f, "RPE\n"); break;
	case 0xe9: fprintf(f, "PCHL\n"); break;
	case 0xea: fprintf(f, "JPE    $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xeb: fprintf(f, "XCHG\n"); break;
	case 0xec: fprintf(f, "CPE     $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xed: fprintf(f, "CALL   $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xee: fprintf(f, "XRI    #$%02x\n", code[1]); opbytes = 2; break;
	case 0xef: fprintf(f, "RST    5\n"); break;

	case 0xf0: fprintf(f, "RP\n");  break;
	case 0xf1: fprintf(f, "POP    PSW\n"); break;
	case 0xf2: fprintf(f, "JP     $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xf3: fprintf(f, "DI\n");  break;
	case 0xf4: fprintf(f, "CP     $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xf5: fprintf(f, "PUSH   PSW\n"); break;
	case 0xf6: fprintf(f, "ORI    #$%02x\n", code[1]); opbytes = 2; break;
	case 0xf7: fprintf(f, "RST    6\n"); break;
	case 0xf8: fprintf(f, "RM\n");  break;
	case 0xf9: fprintf(f, "SPHL\n"); break;
	case 0xfa: fprintf(f, "JM     $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xfb: fprintf(f, "EI\n");  break;
	case 0xfc: fprintf(f, "CM     $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xfd: fprintf(f, "CALL   $%02x%02x\n", code[2], code[1]); opbytes = 3; break;
	case 0xfe: fprintf(f, "CPI    #$%02x\n", code[1]); opbytes = 2; break;
	case 0xff: fprintf(f, "RST    7\n"); break;
	}

	return opbytes;
}
