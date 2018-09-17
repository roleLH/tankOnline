#pragma once
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

// 获取全局变量句柄
CmdWindow& staticWindowHandle();

