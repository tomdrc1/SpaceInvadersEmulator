#include "audio.h"

static Mix_Chunk* sounds[MAX_SOUNDS] = { NULL };

void audioInit()
{
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024))
	{
		printf("Couldn't initialize SDL_Mixer: %s\n", Mix_GetError());
	}
}

void audioQuit()
{
	int i = 0;

	for (i = 0; i < MAX_SOUNDS; ++i)
	{
		if (sounds[i])
		{
			freeSound(i);
		}
	}

	Mix_Quit();
}

int loadSound(char* sampleName)
{
	int i = 0;

	for (i = 0; i < MAX_SOUNDS; i++) 
	{
		if (sounds[i] == NULL) 
		{
			sounds[i] = Mix_LoadWAV(sampleName);
			if (sounds[i] == NULL)
			{
				printf("unable to load file '%s'\n", sampleName);
				return -1;
			}
			return i;
		}
	}

	printf("unable to load file (%d file limit reached)\n", MAX_SOUNDS);
	return -1;
}

int playSound(int id)
{
	if (id < 0 || id >= MAX_SOUNDS) 
	{
		return 1;
	}
	return Mix_PlayChannel(-1, sounds[id], 0);
}

void freeSound(int id) 
{
	if (sounds[id] != NULL) 
	{
		Mix_FreeChunk(sounds[id]);
		sounds[id] = NULL;
	}
}

void setVolume(unsigned char volume)
{
	int i = 0;

	for (i = 0; i < MAX_SOUNDS; ++i)
	{
		if (sounds[i])
		{
			if (volume <= 128) 
			{
				sounds[i]->volume = volume;
			}
		}
	}
}

