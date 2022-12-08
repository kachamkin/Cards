#include <SDL.h>
#include <SDL_image.h>
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

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;
const int TOP_BORDER = 40;
const int ANIMATION_LENGTH = 100;
const int CARD_W = 140;
const int CARD_H = 190;
const int BETWEEN_CARDS = 40;
const int HORIZ_STEP = 40;
const string cardsDir = "..\\..\\sdl2-hearts-game-master\\res\\Cards\\";
const string pile = "..\\..\\sdl2-hearts-game-master\\res\\Cards\\cardBack_blue4.png";

bool init();
bool loadMedia(string path, SDL_Surface** gPNGSurface);
void drawCard(string path);
void close();
void updateState();

SDL_Surface* loadSurface(string path);
SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
int initPos = 0;

vector<SDL_Surface> activeSurfs;
vector<int> activePos;
vector<SDL_Rect> activeRects;
vector<string> deck;

bool init()
{
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		success = false;
	else
	{
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

bool loadMedia(string path, SDL_Surface** gPNGSurface)
{
	bool success = true;

	*gPNGSurface = loadSurface(path);
	if (*gPNGSurface == NULL)
		success = false;

	if (success)
		activeSurfs.push_back(**gPNGSurface);

	return success;
}

void close()
{
	for (auto& s : activeSurfs)
		SDL_FreeSurface(&s);

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

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
	if (e->type == SDL_MOUSEBUTTONDOWN)
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

		if (inside && deck.size())
		{
			drawCard(deck[deck.size() - 1]);
			deck.erase(deck.end() - 1);
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
	SDL_UpdateWindowSurface(gWindow);
}

void createDeck()
{
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
}

int main(int argc, char* args[])
{
	if (init())
	{
		SDL_Surface* gPNGSurface = NULL;
		if (loadMedia(pile, &gPNGSurface))
		{
			createDeck();
			
			bool animate = true;
			bool quit = false;
			SDL_Event e;
			
			SDL_Rect rect{};
			activeRects.push_back(rect);
			activeRects[0].x = initPos;
			activeRects[0].y = TOP_BORDER;

			SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, R_COLOR, G_COLOR, B_COLOR));

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