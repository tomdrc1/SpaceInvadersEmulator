#include "cpu.h"

byte pairtyCheck(byte num, byte size)
{
	byte count = 0;
	byte i = 0;

	for (i = 0; i < size; i++)
	{
		if ((num & 0x01) == 0x01)
		{
			count++;
		}

		num >>= 1;
	}

	return (count % 2 == 0);
}

byte emulate8080Op(State8080* state)
{
	byte* instruction = &state->memory[state->pc];

	switch (instruction[0])
	{
		case 0x00:
			break;
		case 0x01:
			state->c = instruction[1];
			state->b = instruction[2];
			state->pc += 2;
			break;
		case 0x02:

			break;
		case 0x03:
			printf("Not implemented!\n");
			break;
		case 0x04:
			{
				state->cc.ac = ((((state->b & 0x0F) + (0x01 & 0x0F)) & 0x10) == 0x10);
				byte res = state->b + 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);
				state->b = res;
			}
			break;
		case 0x05:
			{
				state->cc.ac = (((state->b & 0x0F) - (0x01 & 0x0F)) < 0);
				byte res = state->b - 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);
				state->b = res;
			}
			break;
		case 0x06:
			state->b = instruction[1];
			state->pc++;
			break;
		case 0x07:
			printf("Not implemented!\n");
			break;
		case 0x08:
			break;
		case 0x09:
			printf("Not implemented!\n");
			break;
		case 0x0A:
			printf("Not implemented!\n");
			break;
		case 0x0B:
			printf("Not implemented!\n");
			break;
		case 0x0C:
			printf("Not implemented!\n");
			break;
		case 0x0D:
			printf("Not implemented!\n");
			break;
		case 0x0E:
			printf("Not implemented!\n");
			break;
		case 0x0F:
			printf("Not implemented!\n");
			break;
		case 0x10:
			break;
		case 0x11:
			printf("Not implemented!\n");
			break;
		case 0x12:
			printf("Not implemented!\n");
			break;
		case 0x13:
			printf("Not implemented!\n");
			break;
		case 0x14:
			printf("Not implemented!\n");
			break;
		case 0x15:
			printf("Not implemented!\n");
			break;
		case 0x16:
			printf("Not implemented!\n");
			break;
		case 0x17:
			printf("Not implemented!\n");
			break;
		case 0x18:
			printf("Not implemented!\n");
			break;
	}

	state->pc++;

	return 0;
}