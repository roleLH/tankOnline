#pragma once
#include "Joypad.h"
#include "Object.h"
#include "util.h"

namespace tank_war
{
	struct User
	{
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

		int addUser();

		inline Tank& getTank(int userId)
		{
			return objManager.getTank(userList[userId].tankId);
		}
		inline Joypad& getJoypad(int userId)
		{
			return joypadManager.getJoypad(userList[userId].joypadId);
		}

	private:
		List<User> userList;
		ObjectManager objManager;
		JoypadManager joypadManager;
	};

	int System::addUser()
	{
		int id = userList.newObject();
		if (id != -1)
		{
			userList[id].tankId = objManager.newTank();
			userList[id].joypadId = joypadManager.newJoypad();
			userList[id].isLive = true;
		}
		return id;
	}


	void System::control(const char* data)
	{
		uint8_t numOfUser = data[0];
		for (uint8_t i = 0; i < numOfUser; ++i)
		{
			if (userList[data[i * 7 + 1]].isLive)
			{
				Joypad& joypad = joypadManager.getJoypad(userList[data[i * 7 + 1]].joypadId);
				joypad.set(data + i * 7 + 2);
			}
		}
	}

	void System::update()
	{
		for (size_t i = 0; i < userList.size(); ++i)
		{
			if (userList[i].isLive)
			{
				Joypad& joypad = joypadManager.getJoypad(userList[i].joypadId);
				Tank& tank = objManager.getTank(userList[i].tankId);
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