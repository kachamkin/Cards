#include "header.h"

bool init()
{
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
		success = false;
	else if (TTF_Init() < 0)
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

Uint32 getPixel(SDL_Surface* surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16*)p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32*)p;
		break;

	default:
		return 0;      
	}
}

void setPixel(SDL_Surface* s, int x, int y, Uint32 pixel)
{
	Uint32* const target_pixel = (Uint32*)((Uint8*)s->pixels
		+ y * s->pitch
		+ x * s->format->BytesPerPixel);
	*target_pixel = pixel;
}

void replaceCardColor(SDL_Surface* s, SDL_Color from, SDL_Color to)
{
	SDL_LockSurface(s);
	for (int y = 0; y < s->h; y++)
	{
		for (int x = 0; x < s->w; x++)
		{
			SDL_Color rgb;
			Uint32 pixelData = getPixel(s, x, y);
			SDL_GetRGB(pixelData, s->format, &rgb.r, &rgb.g, &rgb.b);
			if (rgb.r == from.r && rgb.g == from.g && rgb.b == from.b)
				setPixel(s, x, y, SDL_MapRGB(s->format, to.r, to.g, to.b));
		}
	}	
	SDL_UnlockSurface(s);
}

void getRandomBack()
{
	vector<string> backs;
	for (auto const& entry : filesystem::directory_iterator(cardsDir))
	{
		string file = entry.path().string();
		if (file.find("cardBack") != string::npos)
			backs.push_back(file);
	}

	random_device rd;
	mt19937 generator(rd());

	shuffle(backs.begin(), backs.end(), generator);

	SDL_Surface* gPNGSurface = NULL;
	if (loadMedia(backs[0], &gPNGSurface, true))
	{
		SDL_BlitSurface(gPNGSurface, NULL, gScreenSurface, &activeRects[0]);
		activeSurfs[0] = *gPNGSurface;
		SDL_UpdateWindowSurface(gWindow);
	}
}

SDL_Color getRandomColor()
{
	int r, g, b;

	vector<int> vals;
	for (int i = 180; i < 256; i++)
		vals.push_back(i);

	random_device rd;
	mt19937 generator(rd());

	shuffle(vals.begin(), vals.end(), generator);
	r = vals[0];

	shuffle(vals.begin(), vals.end(), generator);
	g = vals[0];

	shuffle(vals.begin(), vals.end(), generator);
	b = vals[0];

	return SDL_Color(r, g, b);
}

void replaceCardsColor()
{
	getRandomBack();
	SDL_Color to = getRandomColor();
	for (int i = 1; i < activeSurfs.size(); i++)
		replaceCardColor(&activeSurfs[i], SDL_Color(cardsColor.r, cardsColor.g, cardsColor.b), to);
	cardsColor = to;
	SDL_UpdateWindowSurface(gWindow);
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

int getValueFromName(string name)
{
	if (name.find("2.png") != string::npos)
		return 2;
	if (name.find("3.png") != string::npos)
		return 3;
	if (name.find("4.png") != string::npos)
		return 4;
	if (name.find("5.png") != string::npos)
		return 5;
	if (name.find("6.png") != string::npos)
		return 6;
	if (name.find("7.png") != string::npos)
		return 7;
	if (name.find("8.png") != string::npos)
		return 8;
	if (name.find("9.png") != string::npos)
		return 9;
	if (name.find("10.png") != string::npos || name.find("J.png") != string::npos || name.find("K.png") != string::npos || name.find("Q.png") != string::npos)
		return 10;
	if (name.find("A.png") != string::npos)
		return 11;
	return 0;
}

void close()
{
	for (auto& s : activeSurfs)
		SDL_FreeSurface(&s);

	if (gFont)
		TTF_CloseFont(gFont);

	if (gMusic)
		Mix_FreeMusic(gMusic);

	SDL_DestroyWindow(gWindow);

	TTF_Quit();
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

		bool insideColor = true;
		if (x < colorRect.x)
		{
			insideColor = false;
		}
		else if (x > colorRect.x + AUDIO_WIDTH)
		{
			insideColor = false;
		}
		else if (y < colorRect.y)
		{
			insideColor = false;
		}
		else if (y > colorRect.y + AUDIO_HEIGHT)
		{
			insideColor = false;
		}

		if (e->type == SDL_MOUSEMOTION)
			SDL_SetCursor(SDL_CreateSystemCursor(inside || insideNewGame || insideAudio || insideColor ? SDL_SYSTEM_CURSOR_HAND : SDL_SYSTEM_CURSOR_ARROW));

		if (e->type == SDL_MOUSEBUTTONDOWN && insideNewGame)
			createDeck();

		if (e->type == SDL_MOUSEBUTTONDOWN && insideAudio && gMusic)
		{
			if (Mix_PausedMusic())
				Mix_ResumeMusic();
			else
				Mix_PauseMusic();
		}

		if (e->type == SDL_MOUSEBUTTONDOWN && insideColor)
			replaceCardsColor();
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
		replaceCardColor(&activeSurfs[activeSurfs.size() - 1], SDL_Color(255, 255, 255), cardsColor);
		SDL_UpdateWindowSurface(gWindow);
	}

	amount += getValueFromName(path);

	updateState();

	if (amount == 21)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "VICTORY!", "Win!", gWindow);
		createDeck();
	}
	if (amount > 21)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Defeat...", "You lose...", gWindow);
		createDeck();
	}
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
	
	amount = 0;
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

	textRect.x = w / 2 - 60;
	textRect.y = h / 2 - 30;

	colorRect.x = 10;
	colorRect.y = h - AUDIO_HEIGHT - 10;

	SDL_Surface* gPNGSurface = NULL;
	if (loadMedia(cardsDir + "/NewGame.png", &gPNGSurface, true))
		SDL_BlitSurface(gPNGSurface, NULL, gScreenSurface, &newGame);
	if (loadMedia(cardsDir + "/SeekPng.com_colorful-png_512093.png", &gPNGSurface, true))
		SDL_BlitSurface(gPNGSurface, NULL, gScreenSurface, &colorRect);
	if (loadMedia(Mix_PausedMusic() ? cardsDir + "/icons8-audio-50.png" : cardsDir + "/icons8-no-audio-50.png", &gPNGSurface, true))
		SDL_BlitSurface(gPNGSurface, NULL, gScreenSurface, &audioRect);
	if (amount && gFont)
	{
		SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, to_string(amount).c_str(), { 255, 255, 255 });
		if (textSurface)
			SDL_BlitSurface(textSurface, NULL, gScreenSurface, &textRect);
	}
}

string getCardsDir(char* arg)
{
	return filesystem::path(arg).parent_path().string() + "/Res";
}

int main(int argc, char* args[])
{
	cardsDir = getCardsDir(args[0]);
	pile = pile = cardsDir + "/cardBack_blue4.png";

	if (init())
	{
		SDL_Surface* gPNGSurface = NULL;
		if (loadMedia(pile, &gPNGSurface))
		{
			bool animate = true;
			bool quit = false;
			SDL_Event e; 

			gFont = TTF_OpenFont((cardsDir + "/Arial-BoldMT.ttf").data(), 64);
			
			Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
			gMusic = Mix_LoadMUS((cardsDir + "/Hugh_Laurie_-_The_Weed_Smokers_Dream_(musmore.com).mp3").data());
			if (gMusic)
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
				SDL_WaitEventTimeout(NULL, EVENT_TIMEOUT);
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