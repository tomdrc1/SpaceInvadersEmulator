all:
	gcc main.c cpu.c audio.c spaceInvadersMachine.c -lSDL2 -lSDL2_mixer -lwiringPi -o machine
