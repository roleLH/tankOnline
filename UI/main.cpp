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
	TankInfo info;
	
	auto oldTime = steady_clock::now();
	while (1)
	{
		if (SDL_PollEvent(&e))
		{

			if (e.type == SDL_KEYDOWN)
			{
				staticControllerHandle().keySwitch(e.key.keysym.sym);
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

					staticControllerHandle().resetMsg();

					int playerId = staticControllerHandle().getPlayerId();
					int userId = staticControllerHandle().getUserId();
					info.setId(userId);
					info.setScore(staticSysyemHandle().getScore(playerId));
					info.setIsLive(staticSysyemHandle().live(playerId));

					Tank& tank = staticSysyemHandle().getTank(playerId);
					info.setPosition(tank.getX(), tank.getY());
					info.setDirection(tank.getDir());
					Joypad& joypad = staticSysyemHandle().getJoypad(playerId);
					info.setJoyPad(joypad.toStr().c_str());

					staticSysyemHandle().update();
					char* bmp = info.getInfoMap();

					CmdWindow& cmdLine = staticWindowHandle();
					for (size_t i = 0; i < 20; ++i)
					{
						for (size_t j = 0; j < 20; ++j)
						{
							cmdLine.drawChar(i + 21, j, bmp[i + j * 20]);
						}
					}
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
