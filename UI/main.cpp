#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <vector>
#include <string>
#include <assert.h>


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
	void text(int x, int y, std::string& text);
	inline void setCursor(const int x, const int y) { cursor.x = x; cursor.y = y; }

	void update();
private:
	SDL_Point cursor;
	int width;
	int height;
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




int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* win = SDL_CreateWindow("demo", 0, 0, 600, 600, 0);
	SDL_Surface* winSurf = SDL_GetWindowSurface(win);

	SDL_Rect rect;
	rect.h = 12;
	rect.w = 12;

	rect.x = 50;
	rect.y = 50;

	Letters letters("font.bmp");

	


	SDL_Surface* l = letters.getSurf('h');

	SDL_BlitSurface(l, NULL, winSurf, &rect);

	SDL_UpdateWindowSurface(win);

	SDL_Delay(3000);
	SDL_Quit();

	return 0;
}