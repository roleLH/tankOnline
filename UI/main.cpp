#define SDL_MAIN_HANDLED
#include <iostream>

#include "UI.h"
#include "System.h"
#include "CmdWindow.h"
#include "Object.h"
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

	char* buf = staticControllerHandle().getSendBuf();

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
					buf[2] = 1;
					break;
				case SDLK_a:
					buf[4] = 1;
					break;
				case SDLK_s:
					buf[3] = 1;
					break;
				case SDLK_d:
					buf[5] = 1;
					break;

				case SDLK_4:
					staticControllerHandle().createServer();
					break;
				case SDLK_5:
					staticControllerHandle().clientLink();
					break;
				case SDLK_6:
					staticControllerHandle().gameLoop();
					break;
				default:
					break;
				}
			}
			else if (e.type == SDL_QUIT)
			{
				staticControllerHandle().finish();
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

				if (staticControllerHandle().isStart())
				{
					staticControllerHandle().sendMsg();
					char* recvBuf = staticControllerHandle().getRecvBuf();
					staticSysyemHandle().control(recvBuf);
					staticSysyemHandle().update();
					staticControllerHandle().resetMsg();

				}

				SMap& map = staticMapHandle().getMap();
				CmdWindow& cmdLine = staticWindowHandle();

				for (int i = 0; i < MAP_WIDTH; ++i)
				{
					for (int j = 0; j < MAP_HEIGHT; ++j)
					{
						cmdLine.drawChar(i, MAP_HEIGHT - j -1, map.map[i][j]);
					}
				}

			}
		}
		staticWindowHandle().update();
	}


	WSACleanup();
	return 0;
}
