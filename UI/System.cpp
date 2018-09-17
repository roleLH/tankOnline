#include "System.h"

namespace tank_war
{

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


	System& staticSysyemHandle()
	{
		static System staticSystem;
		return staticSystem;
	}

}