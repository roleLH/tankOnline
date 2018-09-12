#define SDL_MAIN_HANDLED
#include <iostream>

#include "UI.h"
#include "System.h"
#include "CmdWindow.h"
using namespace::UI;
using namespace::tank_war;



int main(int argc, char** argv)
{
	
	SDL_Init(SDL_INIT_EVENTS);


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

	TankInfo info;
	char* infoMap = info.getInfoMap();

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
				case SDLK_j:
					buf[6] = 1; break;
				case SDLK_UP:
					buf[9] = 1;	break;
				case SDLK_DOWN:
					buf[10] = 1; break;
				case SDLK_LEFT:
					buf[11] = 1; break;
				case SDLK_RIGHT:
					buf[12] = 1; break;
				case SDLK_0:
					buf[13] = 1; break;
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
		cmdLine.text(20, 33, "joypad:");
		cmdLine.text(28, 33, buf);
		

		for (size_t i{}; i < 16; ++i)
			buf[i] = 0;
		buf[0] = 2;
		buf[1] = userId;
		buf[8] = user2;

		for (size_t x = 0; x < MAP_WIDTH; ++x)
		{
			for (size_t y = 0; y < MAP_HEIGHT; ++y)
			{
				cmdLine.drawChar(x, MAP_HEIGHT - y - 1, map[x][y]);
			}
		}

		info.setId(userId);
		Tank& tank = system.getTank(userId);
		info.setPosition(tank.getX(), tank.getY());
		info.setDirection(tank.getDir());
		info.setIsLive(tank.getIsLive());

		for (size_t i = 0; i < 20; ++i)
			for (size_t j = 0; j < 20; ++j)
				cmdLine.drawChar(j + 20, i, infoMap[i * 20 + j]);


		cmdLine.update();

	//	resetMap();
	//	SDL_Delay(33);
	}

	
	
	

	return 0;
}