#pragma once
#include <unordered_map>
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

		inline int getPlayerId(int userId)
		{
			auto iter = userMap.find(userId);
			if (iter == userMap.end())
				return -1;
			return iter->second;
		}

		inline Tank& getTank(int playerId)
		{
			return objManager.getTank(playerList[playerId].tankId);
		}
		inline Joypad& getJoypad(int playerId)
		{
			return joypadManager.getJoypad(playerList[playerId].joypadId);
		}

		inline int getScore(int playerId) { return playerList[playerId].score; }
		inline bool live(int playerId) { return playerList[playerId].isLive; }
	private:

		// 我们需要提供一个用户id到playerid的映射
		// 具体的原因：我们需要同步其他用户的信息，但是我们不能保证所有用户在所有客户端具有相同的playerId。
		// 但是可以保证的是，所有用户具有一个相同的userId，所以我们用userId去映射playerId
		std::unordered_map<int, int>userMap;	
		List<Player> playerList;
		ObjectManager objManager;
		JoypadManager joypadManager;
	};

	System& staticSysyemHandle();

}