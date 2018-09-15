#pragma once
#include "Joypad.h"
#include "Object.h"
#include "util.h"

namespace tank_war
{
	struct Player
	{
		int userId;
		int tankId;
		int joypadId;
		int score;
		bool isLive;
	};

	class System
	{
	public:
		void control(const char* );
		void update();

		int addPlayer(int userId);	// 关于userId参数，目的是为了对应哪一个用户（网络连接时使用）

		inline Tank& getTank(int playerId)
		{
			return objManager.getTank(playerList[playerId].tankId);
		}
		inline Joypad& getJoypad(int playerId)
		{
			return joypadManager.getJoypad(playerList[playerId].joypadId);
		}

	private:
		List<Player> playerList;
		ObjectManager objManager;
		JoypadManager joypadManager;
	};

	static System staticSystem;

	int System::addPlayer(int userId)
	{
		int id = playerList.newObject();
		if (id != -1)
		{
			playerList[id].userId = userId;
			playerList[id].tankId = objManager.newTank();
			playerList[id].joypadId = joypadManager.newJoypad();
			playerList[id].isLive = true;
		}
		return id;
	}


	void System::control(const char* data)
	{
		uint8_t numOfPlayer = data[0];
		for (uint8_t i = 0; i < numOfPlayer; ++i)
		{
			if (playerList[data[i * 7 + 1]].isLive)
			{
				Joypad& joypad = joypadManager.getJoypad(playerList[data[i * 7 + 1]].joypadId);
				joypad.set(data + i * 7 + 2);
			}
		}
	}

	void System::update()
	{
		for (size_t i = 0; i < playerList.size(); ++i)
		{
			if (playerList[i].isLive)
			{
				Joypad& joypad = joypadManager.getJoypad(playerList[i].joypadId);
				Tank& tank = objManager.getTank(playerList[i].tankId);
				if (joypad.isPress(KEY_UP))
					tank.up();
				else if (joypad.isPress(KEY_DOWN))
					tank.down();
				else if (joypad.isPress(KEY_LEFT))
					tank.left();
				else if (joypad.isPress(KEY_RIGHT))
					tank.right();
				if (joypad.isPress(KEY_OK))	// 这里存在多次复制，可能会有影响
				{
					Bullet bullet = tank.fire();
					int bid = objManager.newBullet();
					Bullet& tb = objManager.getBullet(bid);
					tb = bullet;
				}
					
				joypad.reset();
			}
		}
		objManager.update();
	}

}