#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <vector>
#include <string>
#include <assert.h>
#include <iostream>

#include "UI.h"
#include "System.h"
using namespace::UI;
using namespace::tank_war;


class Letters
{
public:
	Letters(const std::string& file);
	~Letters();

	inline SDL_Surface* getSurf(uint8_t ch) const { return letterSurfs[ch]; }

private:
	std::vector<SDL_Surface*> letterSurfs;
};

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

class CmdWindow
{
public:
	CmdWindow(int x = 0, int y = 0, int w = 60, int h = 40);
	~CmdWindow();
	void text(int x, int y, const std::string& text);
	void drawChar(int x, int y, uint8_t ch);
	void print(const std::string& text);
	inline void setCursor(const int x, const int y) { cursor.x = x; cursor.y = y; }

	void update();
private:
	SDL_Point cursor;
	int width;
	int height;
	int lw;
	int lh;
	SDL_Window* win;
	SDL_Surface* winSurf;
	Letters* letters;
};

CmdWindow::CmdWindow(int x, int y, int w, int h)
{
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
}

void CmdWindow::text(int x, int y, const std::string& text)
{
	cursor.x = x;
	cursor.y = y;
	print(text);
}

void CmdWindow::drawChar(int y, int x, uint8_t ch)
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


int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Init(SDL_INIT_EVENTS);

	CmdWindow cmdLine;
	

	bool isFinished = false;

	char buf[17] = {0, };
	System system;
	int userId = system.addUser();
	int user2 = system.addUser();
	buf[0] = 2;
	buf[1] = userId;
	buf[8] = user2;
	buf[2] = 1;
	buf[16] = 0;

	SDL_Event e;

	while (!isFinished)
	{
		if (SDL_PollEvent(&e))
		{
			if (e.type == SDL_KEYUP)
			{
				
			}
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_w :
					buf[2] = 1;	break;
				case SDLK_a :
					buf[4] = 1; break;
				case SDLK_s : 
					buf[3] = 1; break;
				case SDLK_d :
					buf[5] = 1; break;
				case SDLK_UP:
					buf[9] = 1;	break;
				case SDLK_DOWN:
					buf[10] = 1; break;
				case SDLK_LEFT:
					buf[11] = 1; break;
				case SDLK_RIGHT:
					buf[12] = 1; break;

				default:
					break;
				}
			}
			else if (e.type == SDL_QUIT)
				break;
		}
		system.control(buf);
		system.update();


		for (size_t i{}; i < 16; ++i)
			buf[i] += '0';
		cmdLine.text(20, 19, "joypad:");
		cmdLine.text(28, 19, buf);
		

		for (size_t i{}; i < 16; ++i)
			buf[i] = 0;
		buf[0] = 2;
		buf[1] = userId;
		buf[8] = user2;

		for (size_t x = 0; x < MAP_WIDTH; ++x)
		{
			for (size_t y = 0; y < MAP_HEIGHT; ++y)
			{
				cmdLine.drawChar(MAP_HEIGHT - y - 1, x, map[x][y]);
			}
		}

		cmdLine.update();

	//	resetMap();
		SDL_Delay(33);
	}

	
	
	SDL_Quit();

	return 0;
}