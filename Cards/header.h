#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include "SDL_mixer.h"
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <thread>
#include <filesystem>
#include <random>

using namespace std;

#define R_COLOR 0 
#define G_COLOR 100 
#define B_COLOR 0 

#define EVENT_TIMEOUT 64 

#define NEW_GAME_WIDTH 137 
#define NEW_GAME_HEIGHT 30 
#define AUDIO_WIDTH 30 
#define AUDIO_HEIGHT 30 

#define SCREEN_WIDTH 1080 
#define SCREEN_HEIGHT 720 
#define TOP_BORDER 40 
#define ANIMATION_LENGTH 100 
#define CARD_W 140 
#define CARD_H 190 
#define BETWEEN_CARDS 40 
#define HORIZ_STEP 40

string cardsDir = "";
string pile = "";

bool init();
bool loadMedia(string path, SDL_Surface** gPNGSurface, bool noAdd);
void drawCard(string path);
void close();
void updateState();
void drawNewGame();
void createDeck();
int getValueFromName(string name);
SDL_Surface* loadSurface(string path);

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
int initPos = 0;

Mix_Music* gMusic = NULL;

vector<SDL_Surface> activeSurfs;
vector<int> activePos;
vector<SDL_Rect> activeRects;
vector<string> deck;

SDL_Rect newGame;
SDL_Rect audioRect;
SDL_Rect textRect;

TTF_Font* gFont = NULL;
int amount = 0;

