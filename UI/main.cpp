#define SDL_MAIN_HANDLED
#include <iostream>

#include "UI.h"
#include "System.h"
#include "CmdWindow.h"

#include "Controller.h"

#include <thread>
#include <mutex>
using namespace::UI;
using namespace::tank_war;
using namespace::net;


int main(int argc, char** argv)
{
	
	WSAData data;
	WSAStartup(MAKEWORD(2, 2), &data);

	SDL_Init(SDL_INIT_EVENTS);

	
	SDL_Event e;


	auto oldTime = steady_clock::now();

	while (1)
	{
		if (SDL_PollEvent(&e))
		{

			if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_w:
					
					break;
				case SDLK_a:
					
					break;
				case SDLK_s:
					
					break;
				case SDLK_d:
					
					break;

				case SDLK_4:
					
					break;
				case SDLK_5:
					
					break;
				case SDLK_6:
					
					break;
				default:
					break;
				}
			}
			else if (e.type == SDL_QUIT)
			{
				break;
			}
				
		}
		else
		{
			auto newTime = steady_clock::now();
			auto d = newTime - oldTime;
			if (duration_cast<milliseconds>(d).count() >= 30)
			{
				oldTime = newTime;
				

				for (int i = 0; i < MAP_HEIGHT; ++i)
				{
					for (int j = 0; j < MAP_WIDTH; ++j)
					{
						 staticWindowHandle().drawChar(i, j, map[j][i]);
					}
				}
				
			}
		}
		staticWindowHandle().update();
	}


	WSACleanup();
	return 0;
}
