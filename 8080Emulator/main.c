#include "machine.h"
#include <time.h>
#include <SDL.h>

int main(int argc, char** argv)
{
	spaceInvaderMachine* machine = (spaceInvaderMachine*)malloc(sizeof(spaceInvaderMachine));

	SDL_Window* screen;
	SDL_Renderer* renderer;
	SDL_Event event;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_CreateWindowAndRenderer(224, 256, 0, &screen, &renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	machine->screen = screen;
	machine->renderer = renderer;
	startEmulation(machine);
	free(machine);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(screen);
	SDL_Quit();
}