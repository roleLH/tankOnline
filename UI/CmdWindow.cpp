#include "CmdWindow.h"

Letters::Letters(const std::string& file)
{
	SDL_Surface* surf = SDL_LoadBMP(file.c_str());
	assert(surf);
	int lh = surf->h >> 4;
	int lw = surf->w >> 4;

	letterSurfs.resize(256);
	for (size_t i = 0; i < 256; ++i)
	{
		letterSurfs[i] = SDL_CreateRGBSurface(0, lw, lh, 32, 0, 0, 0, 0);
	}

	SDL_Rect sRect;
	sRect.h = lh;
	sRect.w = lw;

	for (size_t i = 0; i < 16; ++i)
	{
		for (size_t j = 0; j < 16; ++j)
		{
			sRect.x = j * sRect.w;
			sRect.y = i * sRect.h;
			SDL_BlitSurface(surf, &sRect, letterSurfs[(i << 4) + j], NULL);
		}
	}
	SDL_FreeSurface(surf);
}

Letters::~Letters()
{
	for (auto& surf : letterSurfs)
	{
		if (surf)
			SDL_FreeSurface(surf);
	}
}




CmdWindow::CmdWindow(int x, int y, int w, int h)
{
	SDL_Init(SDL_INIT_VIDEO);

	cursor.x = x;
	cursor.y = y;
	width = w;
	height = h;
	letters = new Letters("font.bmp");
	assert(letters);
	lw = letters->getSurf(' ')->w;
	lh = letters->getSurf(' ')->h;
	win = SDL_CreateWindow("demo", 50, 50, lw * w, lh * h, 0);
	assert(win);
	winSurf = SDL_GetWindowSurface(win);
	assert(winSurf);
}

CmdWindow::~CmdWindow()
{
	if (letters)
		delete letters;
	if (winSurf)
		SDL_FreeSurface(winSurf);
	if (win)
		SDL_DestroyWindow(win);
	SDL_Quit();
}

void CmdWindow::text(int x, int y, const std::string& text)
{
	cursor.x = x;
	cursor.y = y;
	print(text);
}

void CmdWindow::drawChar(int x, int y, uint8_t ch)
{
	SDL_Rect rect;
	rect.h = lh;
	rect.w = lw;
	rect.y = lw * y;
	rect.x = lh * x;
	SDL_BlitSurface(letters->getSurf(ch), NULL, winSurf, &rect);
}

void CmdWindow::print(const std::string& text)
{
	SDL_Rect rect;
	rect.h = lh;
	rect.w = lw;
	for (size_t i = 0; i < text.length(); ++i)
	{
		rect.x = lw * (cursor.x + i);
		rect.y = lh * cursor.y;
		SDL_Surface* surf = letters->getSurf(text[i]);
		SDL_BlitSurface(surf, NULL, winSurf, &rect);
	}
	cursor.x++;
	cursor.y++;
}

void CmdWindow::update()
{
	SDL_UpdateWindowSurface(win);
}

CmdWindow& staticWindowHandle()
{
	static  CmdWindow cmdLine;
	return cmdLine;
}