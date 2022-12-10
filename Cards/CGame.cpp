//#include "CGame.h"
//
//CGame::CGame(SDL_Window* _gWindow, SDL_Surface* _gScreenSurface, TTF_Font* _gFont)
//{
//	m_gWindow = _gWindow;
//	m_gScreenSurface = _gScreenSurface;
//	m_gFont = _gFont;
//}
//
//void CGame::UpdateState()
//{
//	SDL_FillRect(m_gScreenSurface, NULL, SDL_MapRGB(m_gScreenSurface->format, R_COLOR, G_COLOR, B_COLOR));
//	for (int i = 0; i < activeSurfs.size(); i++)
//		SDL_BlitSurface(&activeSurfs[i], NULL, m_gScreenSurface, &activeRects[i]);
//	DrawNewGame();
//	SDL_UpdateWindowSurface(m_gWindow);
//}
//
//void CGame::CreateDeck()
//{
//	size_t size = activeSurfs.size();
//	if (size > 1)
//	{
//		for (int i = 1; i < size; i++)
//		{
//			activeSurfs.erase(activeSurfs.end() - 1);
//			activePos.erase(activePos.end() - 1);
//			activeRects.erase(activeRects.end() - 1);
//		}
//	}
//
//	random_device rd;
//	mt19937 generator(rd());
//
//	amount = 0;
//	deck.clear();
//	for (auto const& entry : filesystem::directory_iterator(cardsDir))
//	{
//		string sEntry = entry.path().string();
//		if (sEntry.find("cardBack") == string::npos)
//			deck.push_back(sEntry);
//	}
//
//	shuffle(deck.begin(), deck.end(), generator);
//
//	UpdateState();
//}
//
//void CGame::DrawNewGame()
//{
//	int w, h;
//	SDL_GetWindowSize(m_gWindow, &w, &h);
//
//	newGame.x = (w - NEW_GAME_WIDTH) / 2;
//	newGame.y = h - NEW_GAME_HEIGHT - 10;
//
//	audioRect.x = w - AUDIO_WIDTH - 20;
//	audioRect.y = h - AUDIO_HEIGHT - 10;
//
//	textRect.x = w / 2 - 60;
//	textRect.y = h / 2 - 30;
//
//	SDL_Surface* gPNGSurface = NULL;
//	if (loadMedia(cardsDir + "/NewGame.png", &gPNGSurface, true))
//		SDL_BlitSurface(gPNGSurface, NULL, m_gScreenSurface, &newGame);
//	if (loadMedia(Mix_PausedMusic() ? cardsDir + "/icons8-audio-50.png" : cardsDir + "/icons8-no-audio-50.png", &gPNGSurface, true))
//		SDL_BlitSurface(gPNGSurface, NULL, m_gScreenSurface, &audioRect);
//	if (amount && gFont)
//	{
//		SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, to_string(amount).c_str(), { 255, 255, 255 });
//		if (textSurface)
//			SDL_BlitSurface(textSurface, NULL, m_gScreenSurface, &textRect);
//	}
//}
//
//
