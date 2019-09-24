#include "spaceInvadersMachine.h"
#include <time.h>
#include <SDL2/SDL.h>

void setupMachine(spaceInvaderMachine* machine);

int main(int argc, char** argv)
{
	spaceInvaderMachine* machine = (spaceInvaderMachine*)malloc(sizeof(spaceInvaderMachine));
	setupMachine(machine);

	startEmulation(machine);

	SDL_DestroyRenderer(machine->renderer);
	SDL_DestroyWindow(machine->screen);
	free(machine);
	SDL_Quit();
}

void setupMachine(spaceInvaderMachine* machine)
{
	SDL_Window* screen;
	SDL_Renderer* renderer;
	SDL_Event event;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);

	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &screen, &renderer);
	SDL_ShowCursor(SDL_DISABLE);

	machine->screen = screen;
	machine->renderer = renderer;
}
