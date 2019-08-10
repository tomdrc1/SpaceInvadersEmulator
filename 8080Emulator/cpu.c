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
			printf("Not implemented!\n");
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
			state->cc.cy = (state->a & 0x80) == 0x80;
			state->a <<= 1;
			state->a = ((state->a & 0xFE) | state->cc.cy);
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
			{
				state->cc.ac = ((((state->c & 0x0F) + (0x01 & 0x0F)) & 0x10) == 0x10);
				byte res = state->c + 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);
				state->c = res;
			}
			break;
		case 0x0D:
			{
				state->cc.ac = (((state->c & 0x0F) - (0x01 & 0x0F)) < 0);
				byte res = state->c - 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);
				state->c = res;
			}
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
		case 0x19:
			printf("Not implemented!\n");
			break;
		case 0x1A:
			printf("Not implemented!\n");
			break;
		case 0x1B:
			printf("Not implemented!\n");
			break;
		case 0x1C:
			printf("Not implemented!\n");
			break;
		case 0x1D:
			printf("Not implemented!\n");
			break;
		case 0x1E:
			printf("Not implemented!\n");
			break;
		case 0x1F:
			printf("Not implemented!\n");
			break;
		case 0x20:
			printf("Not implemented!\n");
			break;
		case 0x21:
			printf("Not implemented!\n");
			break;
		case 0x22:
			printf("Not implemented!\n");
			break;
		case 0x23:
			printf("Not implemented!\n");
			break;
		case 0x24:
			printf("Not implemented!\n");
			break;
		case 0x25:
			printf("Not implemented!\n");
			break;
		case 0x26:
			printf("Not implemented!\n");
			break;
		case 0x27:
			printf("Not implemented!\n");
			break;
		case 0x28:
			printf("Not implemented!\n");
			break;
		case 0x29:
			printf("Not implemented!\n");
			break;
		case 0x2A:
			printf("Not implemented!\n");
			break;
		case 0x2B:
			printf("Not implemented!\n");
			break;
		case 0x2C:
			printf("Not implemented!\n");
			break;
		case 0x2D:
			printf("Not implemented!\n");
			break;
		case 0x2E:
			printf("Not implemented!\n");
			break;
		case 0x2F:
			printf("Not implemented!\n");
			break;
		case 0x30:
			printf("Not implemented!\n");
			break;
		case 0x31:
			printf("Not implemented!\n");
			break;
		case 0x32:
			printf("Not implemented!\n");
			break;
		case 0x33:
			printf("Not implemented!\n");
			break;
		case 0x34:
			printf("Not implemented!\n");
			break;
		case 0x35:
			printf("Not implemented!\n");
			break;
		case 0x36:
			printf("Not implemented!\n");
			break;
		case 0x37:
			printf("Not implemented!\n");
			break;
		case 0x38:
			printf("Not implemented!\n");
			break;
		case 0x39:
			printf("Not implemented!\n");
			break;
		case 0x3A:
			printf("Not implemented!\n");
			break;
		case 0x3B:
			printf("Not implemented!\n");
			break;
		case 0x3C:
			printf("Not implemented!\n");
			break;
		case 0x3D:
			printf("Not implemented!\n");
			break;
		case 0x3E:
			state->a = instruction[1];
			state->pc++;
			break;
		case 0x3F:
			state->cc.cy = !state->cc.cy;
			break;
		case 0x40:
			printf("Not implemented!\n");
			break;
		case 0x41:
			printf("Not implemented!\n");
			break;
		case 0x42:
			printf("Not implemented!\n");
			break;
		case 0x43:
			printf("Not implemented!\n");
			break;
		case 0x44:
			printf("Not implemented!\n");
			break;
		case 0x45:
			printf("Not implemented!\n");
			break;
		case 0x46:
			printf("Not implemented!\n");
			break;
		case 0x47:
			printf("Not implemented!\n");
			break;
		case 0x48:
			printf("Not implemented!\n");
			break;
		case 0x49:
			printf("Not implemented!\n");
			break;
		case 0x4A:
			printf("Not implemented!\n");
			break;
		case 0x4B:
			printf("Not implemented!\n");
			break;
		case 0x4C:
			printf("Not implemented!\n");
			break;
		case 0x4D:
			printf("Not implemented!\n");
			break;
		case 0x4E:
			printf("Not implemented!\n");
			break;
		case 0x4F:
			printf("Not implemented!\n");
			break;
		case 0x50:
			printf("Not implemented!\n");
			break;
		case 0x51:
			printf("Not implemented!\n");
			break;
		case 0x52:
			printf("Not implemented!\n");
			break;
		case 0x53:
			printf("Not implemented!\n");
			break;
		case 0x54:
			printf("Not implemented!\n");
			break;
		case 0x55:
			printf("Not implemented!\n");
			break;
		case 0x56:
			printf("Not implemented!\n");
			break;
		case 0x57:
			printf("Not implemented!\n");
			break;
		case 0x58:
			printf("Not implemented!\n");
			break;
		case 0x59:
			printf("Not implemented!\n");
			break;
		case 0x5A:
			printf("Not implemented!\n");
			break;
		case 0x5B:
			printf("Not implemented!\n");
			break;
		case 0x5C:
			printf("Not implemented!\n");
			break;
		case 0x5D:
			printf("Not implemented!\n");
			break;
		case 0x5E:
			printf("Not implemented!\n");
			break;
		case 0x5F:
			printf("Not implemented!\n");
			break;
		case 0x60:
			printf("Not implemented!\n");
			break;
		case 0x61:
			printf("Not implemented!\n");
			break;
		case 0x62:
			printf("Not implemented!\n");
			break;
		case 0x63:
			printf("Not implemented!\n");
			break;
		case 0x64:
			printf("Not implemented!\n");
			break;
		case 0x65:
			printf("Not implemented!\n");
			break;
		case 0x66:
			printf("Not implemented!\n");
			break;
		case 0x67:
			printf("Not implemented!\n");
			break;
		case 0x68:
			printf("Not implemented!\n");
			break;
		case 0x69:
			printf("Not implemented!\n");
			break;
		case 0x6A:
			printf("Not implemented!\n");
			break;
		case 0x6B:
			printf("Not implemented!\n");
			break;
		case 0x6C:
			printf("Not implemented!\n");
			break;
		case 0x6D:
			printf("Not implemented!\n");
			break;
		case 0x6E:
			printf("Not implemented!\n");
			break;
		case 0x6F:
			printf("Not implemented!\n");
			break;
		case 0x70:
			printf("Not implemented!\n");
			break;
		case 0x71:
			printf("Not implemented!\n");
			break;
		case 0x72:
			printf("Not implemented!\n");
			break;
		case 0x73:
			printf("Not implemented!\n");
			break;
		case 0x74:
			printf("Not implemented!\n");
			break;
		case 0x75:
			printf("Not implemented!\n");
			break;
		case 0x76:
			printf("Not implemented!\n");
			break;
		case 0x77:
			printf("Not implemented!\n");
			break;
		case 0x78:
			printf("Not implemented!\n");
			break;
		case 0x79:
			printf("Not implemented!\n");
			break;
		case 0x7A:
			printf("Not implemented!\n");
			break;
		case 0x7B:
			printf("Not implemented!\n");
			break;
		case 0x7C:
			printf("Not implemented!\n");
			break;
		case 0x7D:
			printf("Not implemented!\n");
			break;
		case 0x7E:
			printf("Not implemented!\n");
			break;
		case 0x7F:
			printf("Not implemented!\n");
			break;
		case 0x80:
			printf("Not implemented!\n");
			break;
		case 0x81:
			printf("Not implemented!\n");
			break;
		case 0x82:
			printf("Not implemented!\n");
			break;
		case 0x83:
			printf("Not implemented!\n");
			break;
		case 0x84:
			printf("Not implemented!\n");
			break;
		case 0x85:
			printf("Not implemented!\n");
			break;
		case 0x86:
			printf("Not implemented!\n");
			break;
		case 0x87:
			printf("Not implemented!\n");
			break;
		case 0x88:
			printf("Not implemented!\n");
			break;
		case 0x89:
			printf("Not implemented!\n");
			break;
		case 0x8A:
			printf("Not implemented!\n");
			break;
		case 0x8B:
			printf("Not implemented!\n");
			break;
		case 0x8C:
			printf("Not implemented!\n");
			break;
		case 0x8D:
			printf("Not implemented!\n");
			break;
		case 0x8E:
			printf("Not implemented!\n");
			break;
		case 0x8F:
			printf("Not implemented!\n");
			break;
		case 0x90:
			printf("Not implemented!\n");
			break;
		case 0x91:
			printf("Not implemented!\n");
			break;
		case 0x92:
			printf("Not implemented!\n");
			break;
		case 0x93:
			printf("Not implemented!\n");
			break;
		case 0x94:
			printf("Not implemented!\n");
			break;
		case 0x95:
			printf("Not implemented!\n");
			break;
		case 0x96:
			printf("Not implemented!\n");
			break;
		case 0x97:
			printf("Not implemented!\n");
			break;
		case 0x98:
			printf("Not implemented!\n");
			break;
		case 0x99:
			printf("Not implemented!\n");
			break;
		case 0x9A:
			printf("Not implemented!\n");
			break;
		case 0x9B:
			printf("Not implemented!\n");
			break;
		case 0x9C:
			printf("Not implemented!\n");
			break;
		case 0x9D:
			printf("Not implemented!\n");
			break;
		case 0x9E:
			printf("Not implemented!\n");
			break;
		case 0x9F:
			printf("Not implemented!\n");
			break;
		case 0xA0:
			printf("Not implemented!\n");
			break;
		case 0xA1:
			printf("Not implemented!\n");
			break;
		case 0xA2:
			printf("Not implemented!\n");
			break;
		case 0xA3:
			printf("Not implemented!\n");
			break;
		case 0xA4:
			printf("Not implemented!\n");
			break;
		case 0xA5:
			printf("Not implemented!\n");
			break;
		case 0xA6:
			printf("Not implemented!\n");
			break;
		case 0xA7:
			printf("Not implemented!\n");
			break;
		case 0xA8:
			printf("Not implemented!\n");
			break;
		case 0xA9:
			printf("Not implemented!\n");
			break;
		case 0xAA:
			printf("Not implemented!\n");
			break;
		case 0xAB:
			printf("Not implemented!\n");
			break;
		case 0xAC:
			printf("Not implemented!\n");
			break;
		case 0xAD:
			printf("Not implemented!\n");
			break;
		case 0xAE:
			printf("Not implemented!\n");
			break;
		case 0xAF:
			printf("Not implemented!\n");
			break;
		case 0xB0:
			printf("Not implemented!\n");
			break;
		case 0xB1:
			printf("Not implemented!\n");
			break;

	}

	state->pc++;

	return 0;
}