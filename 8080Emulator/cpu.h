typedef unsigned char byte;

typedef struct ConditionCodes {
	byte z : 1;
	byte s : 1;
	byte p : 1;
	byte cy : 1;
	byte ac : 1;
	byte pad : 3;
} ConditionCodes;

typedef struct State8080 {
	byte a;
	byte b;
	byte c;
	byte d;
	byte e;
	byte h;
	byte l;
	unsigned short sp;
	unsigned short pc;
	byte* memory;
	ConditionCodes cc;
	byte int_enable;
} State8080;

byte pairtyCheck(byte num, byte size);
byte emulate8080Op(State8080* state);