#pragma once
#include "Joypad.h"
#include "Object.h"
#include "util.h"

namespace tank_war
{
	struct Player
	{
		int userId;		// 网络连接时对应服务系统中返回的userId
		int tankId;		// tankObject Id
		int joypadId;	// 控制器id
		int score;		// 得分
		bool isLive;	// 是否存活
	};

	class System
	{
	public:
		void control(const char* );
		void update();

		// 返回该对象的id 创建失败返回-1
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

	System& staticSysyemHandle();

}