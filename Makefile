all: spaceInvaders

spaceInvaders: main.o spaceInvadersMachine.o cpu.o audio.o
	g++ main.o spaceInvadersMachine.o cpu.o audio.o -o spaceInvaders

main.o:
	gcc -c main.c -lSDL

spaceInvadersMachine.o:
	gcc -c spaceInvadersMachine.c

cpu.o:
	gcc -c cpu.c

audio.o:
	gcc -c audio.c

clean:
	rm -rf *o spaceInvaders
