#include "machine.h"
#include <time.h>
#include <SDL.h>

int main(int argc, char** argv)
{
	spaceInvaderMachine* machine = (spaceInvaderMachine*)malloc(sizeof(spaceInvaderMachine));

	SDL_Window* screen;
	SDL_Renderer* renderer;
	SDL_Event event;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);

	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &screen, &renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_ShowCursor(SDL_DISABLE);

	machine->screen = screen;
	machine->renderer = renderer;
	startEmulation(machine);
	free(machine);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(screen);
	SDL_Quit();
}