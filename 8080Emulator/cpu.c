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
			{
				unsigned short bc = (state->b << 8) | state->c;
				state->memory[bc] = state->a;
			}
			break;
		case 0x03:
			{
				unsigned short bc = (state->b << 8) | state->c;
				bc++;

				state->b = (bc & 0xFF00) >> 8;
				state->c = bc & 0x00FF;
			}
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
			{
				unsigned short bc = (state->b << 8) | state->c;
				unsigned short hl = (state->h << 8) | state->l;

				unsigned int res = hl + bc;

				state->h = (res & 0x0000FF00) >> 8;
				state->l = (res & 0x000000FF);

				state->cc.cy = ((res & 0xFFFF0000) != 0);
			}
			break;
		case 0x0A:
			{
				unsigned short bc = (state->b << 8) | state->c;
				state->a = state->memory[bc];
			}
			break;
		case 0x0B:
			{
				unsigned short bc = (state->b << 8) | state->c;
				bc--;

				state->b = (bc & 0xFF00) >> 8;
				state->c = (bc & 0x00FF);
			}
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
			state->c = instruction[1];
			state->pc++;
			break;
		case 0x0F:
			state->cc.cy = (state->a & 0x01) == 0x01;
			state->a >>= 1;
			state->a = ((state->a & 0x7F) | (state->cc.cy << 7));
			break;
		case 0x10:
			break;
		case 0x11:
			state->e = instruction[1];
			state->d = instruction[2];
			state->pc += 2;
			break;
		case 0x12:
			{
				unsigned short de = (state->d << 8) | state->e;
				state->memory[de] = state->a;
			}
			break;
		case 0x13:
			{
				unsigned short de = (state->d << 8) | state->e;
				de++;

				state->d = (de & 0xFF00) >> 8;
				state->e = de & 0x00FF;
			}
			break;
		case 0x14:
			{
				state->cc.ac = ((((state->d & 0x0F) + (0x01 & 0x0F)) & 0x10) == 0x10);
				byte res = state->d + 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->d = res;
			}
			break;
		case 0x15:
			{
				state->cc.ac = (((state->d & 0x0F) - (0x01 & 0x0F)) < 0);
				byte res = state->d - 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->d = res;
			}
			break;
		case 0x16:
			state->d = instruction[1];
			state->pc++;
			break;
		case 0x17:
			{
				byte prevCY = state->cc.cy;
				state->cc.cy = (state->a & 0x80) == 0x80;
				state->a <<= 1;
				state->a = (state->a & 0xFE) | prevCY;
			}
			break;
		case 0x18:
			break;
		case 0x19:
			{
				unsigned short hl = (state->h << 8) | state->l;
				unsigned short de = (state->d << 8) | state->e;

				unsigned int res = hl + de;

				state->h = (res & 0x0000FF00) >> 8;
				state->l = res & 0x000000FF;

				state->cc.cy = ((res & 0xFFFF0000) != 0);
			}
			break;
		case 0x1A:
			{
				unsigned short de = (state->d << 8) | state->e;
				state->a = state->memory[de];
			}
			break;
		case 0x1B:
			{
				unsigned short de = (state->d << 8) | state->e;
				de--;

				state->d = (de & 0xFF00) >> 8;
				state->e = de & 0x00FF;
			}
			break;
		case 0x1C:
			{
				state->cc.ac = ((((state->e & 0x0F) + (0x01 & 0x0F)) & 0x10) == 0x10);
				byte res = state->e + 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->e = res;
			}
			break;
		case 0x1D:
			{
				state->cc.ac = (((state->e & 0x0F) - (0x01 & 0x01)) < 0);
				byte res = state->e - 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->e = res;
			}
			break;
		case 0x1E:
			state->e = instruction[1];
			state->pc++;
			break;
		case 0x1F:
			{
				byte prevCy = state->cc.cy;
				state->cc.cy = (state->a & 0x01) == 0x01;
				state->a >>= 1;
				state->a = (state->a & 0x7F) | (prevCy << 7);
			}
			break;
		case 0x20:
			break;
		case 0x21:
			state->l = instruction[1];
			state->h = instruction[2];
			state->pc += 2;
			break;
		case 0x22:
			{
				unsigned short adr = (instruction[2] << 8) | instruction[1];
				state->memory[adr] = state->l;
				state->memory[adr + 1] = state->h;
				state->pc += 2;
			}
			break;
		case 0x23:
			{
				unsigned short hl = (state->h << 8) | state->l;
				hl++;

				state->h = (hl & 0xFF00) >> 8;
				state->l = hl & 0x00FF;
			}
			break;
		case 0x24:
			{
				state->cc.ac = (((state->h & 0x0F) + (0x01 & 0x0F) & 0x10) == 0x10);
				byte res = state->h + 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->h = res;
			}
			break;
		case 0x25:
			{
				state->cc.ac = ((state->h & 0x0F) - (0x01 & 0x0F) < 0);
				byte res = state->h - 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->h = res;
			}
			break;
		case 0x26:
			state->h = instruction[1];
			state->pc++;
			break;
		case 0x27:
			{
				if ((state->a & 0x0F) > 0x09 || state->cc.ac == 0x01) 
				{
					state->cc.ac = (((state->a & 0x0F) + (0x06 & 0x0F)) & 0x10) == 0x10;
					state->a += 0x06;
				}
				if (((state->a >> 4) & 0x0F) > 0x09 || state->cc.cy == 0x01)
				{
					state->cc.cy = ((((state->a >> 4) & 0x0F) + (0x06 & 0x0F)) & 0x10) == 0x10;
					state->a = (state->a) + (0x06 << 4);
				}

				state->cc.z = (0 == state->a);
				state->cc.p = pairtyCheck(state->a, 8);
				state->cc.s = (0x80 == (state->a & 0x80));
			}
			break;
		case 0x28:
			break;
		case 0x29:
			{
				unsigned short hl = (state->h << 8) | state->l;
				
				unsigned int res = hl << 1;

				state->h = (res & 0x0000FF00) >> 8;
				state->l = res & 0x000000FF;

				state->cc.cy = ((res & 0xFFFF0000) != 0);
			}
			break;
		case 0x2A:
			{
				unsigned short adr = (instruction[2] << 8) | instruction[1];

				state->l = state->memory[adr];
				state->h = state->memory[adr + 1];
				state->pc += 2;
			}
			break;
		case 0x2B:
			{
				unsigned short hl = (state->h << 8) | state->l;
				hl--;

				state->h = (hl & 0xFF00) >> 8;
				state->l = hl & 0x00FF;
			}
			break;
		case 0x2C:
			{
				state->cc.ac = (((state->l & 0x0F) + (0x01 & 0x0F)) & 0x10) == 0x10;
				byte res = state->l + 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->l = res;
			}
			break;
		case 0x2D:
			{
				state->cc.ac = ((state->l & 0x0F) - (0x01 & 0x0F) < 0);
				byte res = state->l - 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->l = res;
			}
			break;
		case 0x2E:
			state->l = instruction[1];
			state->pc++;
			break;
		case 0x2F:
			state->a = ~state->a;
			break;
		case 0x30:
			break;
		case 0x31:
			state->sp = (instruction[2] << 8) | instruction[1];
			state->pc += 2;
			break;
		case 0x32:
			{
				unsigned short adr = (instruction[2] << 8) | instruction[1];
				state->memory[adr] = state->a;

				state->pc += 2;
			}
			break;
		case 0x33:
			state->sp++;
			break;
		case 0x34:
			{
				unsigned short hl = (state->h << 8) | state->l;
				byte value = state->memory[hl];

				state->cc.ac = (((value & 0x0F) + (0x01 & 0x0F)) & 0x10) == 0x10;
				byte res = value + 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->memory[hl] = res;
			}
			break;
		case 0x35:
			{
				unsigned short hl = (state->h << 8) | state->l;
				byte value = state->memory[hl];

				state->cc.ac = ((value & 0x0F) - (0x01 & 0x0F)) < 0;
				byte res = value - 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->memory[hl] = res;
			}
			break;
		case 0x36:
			{
				unsigned short hl = (state->h << 8) | state->l;
				state->memory[hl] = instruction[1];
				state->pc++;
			}
			break;
		case 0x37:
			state->cc.cy = 0x01;
			break;
		case 0x38:
			break;
		case 0x39:
			{
				unsigned short hl = (state->h << 8) | state->l;

				unsigned int res = hl + state->sp;

				state->h = (res & 0x0000FF00) >> 8;
				state->l = res & 0x000000FF;

				state->cc.cy = ((res & 0xFFFF0000) != 0);
			}
			break;
		case 0x3A:
			{
				unsigned short adr = (instruction[2] << 8) | instruction[1];
				state->a = state->memory[adr];
				state->pc += 2;
			}
			break;
		case 0x3B:
			state->sp--;
			break;
		case 0x3C:
			{
				state->cc.ac = (((state->a & 0x0F) + (0x01 & 0x0F)) & 0x10) == 0x10;
				byte res = state->a + 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->a = res;
			}
			break;
		case 0x3D:
			{
				state->cc.ac = (((state->a & 0x0F) - (0x01 & 0x0F)) < 0);
				byte res = state->a - 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->a = res;
			}
			break;
		case 0x3E:
			state->a = instruction[1];
			state->pc++;
			break;
		case 0x3F:
			state->cc.cy = !state->cc.cy;
			break;
		case 0x40:
			state->b = state->b;
			break;
		case 0x41:
			state->b = state->c;
			break;
		case 0x42:
			state->b = state->d;
			break;
		case 0x43:
			state->b = state->e;
			break;
		case 0x44:
			state->b = state->h;
			break;
		case 0x45:
			state->b = state->l;
			break;
		case 0x46:
			{
				unsigned short hl = (state->h << 8) | state->l;
				state->b = state->memory[hl];
			}
			break;
		case 0x47:
			state->b = state->a;
			break;
		case 0x48:
			state->c = state->b;
			break;
		case 0x49:
			state->c = state->c;
			break;
		case 0x4A:
			state->c = state->d;
			break;
		case 0x4B:
			state->c = state->e;
			break;
		case 0x4C:
			state->c = state->h;
			break;
		case 0x4D:
			state->c = state->l;
			break;
		case 0x4E:
			{
				unsigned short hl = (state->h << 8) | state->l;
				state->c = state->memory[hl];
			}
			break;
		case 0x4F:
			state->c = state->a;
			break;
		case 0x50:
			state->d = state->b;
			break;
		case 0x51:
			state->d = state->c;
			break;
		case 0x52:
			state->d = state->d;
			break;
		case 0x53:
			state->d = state->e;
			break;
		case 0x54:
			state->d = state->h;
			break;
		case 0x55:
			state->d = state->l;
			break;
		case 0x56:
			{
				unsigned short hl = (state->h << 8) | state->l;
				state->d = state->memory[hl];
			}
			break;
		case 0x57:
			state->d = state->a;
			break;
		case 0x58:
			state->e = state->b;
			break;
		case 0x59:
			state->e = state->c;
			break;
		case 0x5A:
			state->e = state->d;
			break;
		case 0x5B:
			state->e = state->e;
			break;
		case 0x5C:
			state->e = state->h;
			break;
		case 0x5D:
			state->e = state->l;
			break;
		case 0x5E:
			{
				unsigned short hl = (state->h << 8) | state->l;
				state->e = state->memory[hl];
			}
			break;
		case 0x5F:
			state->e = state->a;
			break;
		case 0x60:
			state->h = state->b;
			break;
		case 0x61:
			state->h = state->c;
			break;
		case 0x62:
			state->h = state->d;
			break;
		case 0x63:
			state->h = state->e;
			break;
		case 0x64:
			state->h = state->h;
			break;
		case 0x65:
			state->h = state->l;
			break;
		case 0x66:
			{
				unsigned short hl = (state->h << 8) | state->l;
				state->h = state->memory[hl];
			}
			break;
		case 0x67:
			state->h = state->a;
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