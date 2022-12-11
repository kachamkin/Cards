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

#define BACK_MUSIC "/Hugh_Laurie_-_The_Weed_Smokers_Dream_(musmore.com).mp3"
#define USED_FONT "/Arial-BoldMT.ttf"
#define INIT_CARD_BACK "/cardBack_blue4.png"
#define NEW_GAME_BUTTON "/NewGame.png"
#define COLORS_CHANGE_BUTTON "/SeekPng.com_colorful-png_512093.png"
#define AUDIO_BUTTON "/icons8-audio-50.png"
#define NO_AUDIO_BUTTON "/icons8-no-audio-50.png"
#define VICTORY_SOUND "/success-fanfare-trumpets-6185.mp3"
#define DEFEAT_SOUND "/failure-1-89170.mp3"
#define WINDOW_ICON "/SeekPng.com_colorful-png_512093_transp.png"

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
SDL_Rect colorRect;

TTF_Font* gFont = NULL;
int amount = 0;

SDL_Color cardsColor(255, 255, 255);

