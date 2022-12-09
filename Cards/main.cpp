#include <SDL.h>
#include <SDL_image.h>
#include "SDL_mixer.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <thread>
#include <filesystem>
#include <random>

using namespace std;

const int R_COLOR = 0;
const int G_COLOR = 100;
const int B_COLOR = 0;

const int NEW_GAME_WIDTH = 137;
const int NEW_GAME_HEIGHT = 30;
const int AUDIO_WIDTH = 30;
const int AUDIO_HEIGHT = 30;

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;
const int TOP_BORDER = 40;
const int ANIMATION_LENGTH = 100;
const int CARD_W = 140;
const int CARD_H = 190;
const int BETWEEN_CARDS = 40;
const int HORIZ_STEP = 40;
const string cardsDir = "..\\..\\sdl2-hearts-game-master\\res\\Cards\\";
const string pile = cardsDir + "cardBack_blue4.png";

bool init();
bool loadMedia(string path, SDL_Surface** gPNGSurface, bool noAdd);
void drawCard(string path);
void close();
void updateState();
void drawNewGame();
void createDeck();

SDL_Surface* loadSurface(string path);
SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
int initPos = 0;

Mix_Music* gMusic;

vector<SDL_Surface> activeSurfs;
vector<int> activePos;
vector<SDL_Rect> activeRects;
vector<string> deck;

SDL_Rect newGame;
SDL_Rect audioRect;

bool init()
{
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
		success = false;
	else
	{
		Mix_Init(MIX_INIT_MP3);

		gWindow = SDL_CreateWindow("Black Jack", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
			success = false;
		else
		{
			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) & imgFlags))
				success = false;
			else
				gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}

	return success;
}

bool loadMedia(string path, SDL_Surface** gPNGSurface, bool noAdd = false)
{
	bool success = true;

	*gPNGSurface = loadSurface(path);
	if (*gPNGSurface == NULL)
		success = false;

	if (success && !noAdd)
		activeSurfs.push_back(**gPNGSurface);

	return success;
}

void close()
{
	for (auto& s : activeSurfs)
		SDL_FreeSurface(&s);

	Mix_FreeMusic(gMusic);
	SDL_DestroyWindow(gWindow);

	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

SDL_Surface* loadSurface(std::string path)
{
	SDL_Surface* optimizedSurface = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface)
	{
		optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
		SDL_FreeSurface(loadedSurface);
	}

	return optimizedSurface;
}

void handleEvent(SDL_Event* e)
{
	if (e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEMOTION)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		bool inside = true;

		if (x < initPos)
		{
			inside = false;
		}
		else if (x > initPos + CARD_W)
		{
			inside = false;
		}
		else if (y < TOP_BORDER)
		{
			inside = false;
		}
		else if (y > TOP_BORDER + CARD_H)
		{
			inside = false;
		}

		if (e->type == SDL_MOUSEBUTTONDOWN && inside && deck.size())
		{
			drawCard(deck[deck.size() - 1]);
			deck.erase(deck.end() - 1);
		}

		bool insideNewGame = true;
		if (x < newGame.x)
		{
			insideNewGame = false;
		}
		else if (x > newGame.x + NEW_GAME_WIDTH)
		{
			insideNewGame = false;
		}
		else if (y < newGame.y)
		{
			insideNewGame = false;
		}
		else if (y > newGame.y + NEW_GAME_HEIGHT)
		{
			inside = false;
		}

		bool insideAudio = true;
		if (x < audioRect.x)
		{
			insideAudio = false;
		}
		else if (x > audioRect.x + AUDIO_WIDTH)
		{
			insideAudio = false;
		}
		else if (y < audioRect.y)
		{
			insideAudio = false;
		}
		else if (y > audioRect.y + AUDIO_HEIGHT)
		{
			insideAudio = false;
		}

		if (e->type == SDL_MOUSEMOTION)
			SDL_SetCursor(SDL_CreateSystemCursor(inside || insideNewGame || insideAudio ? SDL_SYSTEM_CURSOR_HAND : SDL_SYSTEM_CURSOR_ARROW));

		if (e->type == SDL_MOUSEBUTTONDOWN && insideNewGame)
			createDeck();

		if (e->type == SDL_MOUSEBUTTONDOWN && insideAudio)
		{
			if (Mix_PausedMusic())
				Mix_ResumeMusic();
			else
				Mix_PauseMusic();
		}
	}
}

void drawCard(string path)
{
	SDL_Surface* gPNGSurface = NULL;
	loadMedia(path, &gPNGSurface);

	activePos.push_back(activeRects[0].x);

	SDL_Rect rect = activeRects[0];
	activeRects.push_back(rect);

	while (activeRects[activeRects.size() - 1].x < activeRects[0].x + CARD_W + ANIMATION_LENGTH + (activeRects.size() - 1) * HORIZ_STEP)
	{
		activePos[activePos.size() - 1]++;
		activeRects[activeRects.size() - 1].x = activePos[activePos.size() - 1];

		SDL_BlitSurface(&activeSurfs[activeSurfs.size() - 1], NULL, gScreenSurface, &activeRects[activeRects.size() - 1]);
		SDL_UpdateWindowSurface(gWindow);
	}

	updateState();
}

void updateState()
{
	SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, R_COLOR, G_COLOR, B_COLOR));
	for (int i = 0; i < activeSurfs.size(); i++)
		SDL_BlitSurface(&activeSurfs[i], NULL, gScreenSurface, &activeRects[i]);
	drawNewGame();
	SDL_UpdateWindowSurface(gWindow);
}

void createDeck()
{
	size_t size = activeSurfs.size();
	if (size > 1)
	{
		for (int i = 1; i < size; i++)
		{
			activeSurfs.erase(activeSurfs.end() - 1);
			activePos.erase(activePos.end() - 1);
			activeRects.erase(activeRects.end() - 1);
		}
	}

	random_device rd;
	mt19937 generator(rd());
	
	deck.clear();
	for (auto const& entry : filesystem::directory_iterator(cardsDir))
	{
		string sEntry = entry.path().string();
		if (sEntry.find("cardBack") == string::npos)
			deck.push_back(sEntry);
	}

	shuffle(deck.begin(), deck.end(), generator);

	updateState();
}

void drawNewGame()
{
	int w, h;
	SDL_GetWindowSize(gWindow, &w, &h);
	
	newGame.x = (w - NEW_GAME_WIDTH) / 2;
	newGame.y = h - NEW_GAME_HEIGHT - 10;

	audioRect.x = w - AUDIO_WIDTH - 20;
	audioRect.y = h - AUDIO_HEIGHT - 10;

	SDL_Surface* gPNGSurface = NULL;
	if (loadMedia("NewGame.png", &gPNGSurface, true))
		SDL_BlitSurface(gPNGSurface, NULL, gScreenSurface, &newGame);
	if (loadMedia(Mix_PausedMusic() ? "icons8-audio-50.png" : "icons8-no-audio-50.png", &gPNGSurface, true))
		SDL_BlitSurface(gPNGSurface, NULL, gScreenSurface, &audioRect);
}

int main(int argc, char* args[])
{
	if (init())
	{
		SDL_Surface* gPNGSurface = NULL;
		if (loadMedia(pile, &gPNGSurface))
		{
			bool animate = true;
			bool quit = false;
			SDL_Event e; 
			
			Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
			gMusic = Mix_LoadMUS("Hugh_Laurie_-_The_Weed_Smokers_Dream_(musmore.com).mp3");
			Mix_PlayMusic(gMusic, 1);

			SDL_Rect rect{};
			activeRects.push_back(rect);
			activeRects[0].x = initPos;
			activeRects[0].y = TOP_BORDER;

			createDeck();

			SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, R_COLOR, G_COLOR, B_COLOR));
			drawNewGame();

			while (!quit)
			{
				while (SDL_PollEvent(&e))
				{
					if (e.type == SDL_QUIT)
						quit = true;
					handleEvent(&e);
				}

				if (animate)
				{
					while (activeRects[0].x < ANIMATION_LENGTH)
					{
						initPos++;
						activeRects[0].x = initPos;
						SDL_BlitSurface(gPNGSurface, NULL, gScreenSurface, &activeRects[0]);
						SDL_UpdateWindowSurface(gWindow);
					}
					animate = false;
				}

				updateState();
			}
		}
	}

	close();

	return 0;
}