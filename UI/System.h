#pragma once
#include <unordered_map>
#include "Joypad.h"
#include "Object.h"
#include "util.h"

namespace tank_war
{
	struct Player
	{
		int userId;		// ��������ʱ��Ӧ����ϵͳ�з��ص�userId
		int tankId;		// tankObject Id
		int joypadId;	// ������id
		int score;		// �÷�
		bool isLive;	// �Ƿ���
	};

	class System
	{
	public:
		void control(const char* );
		void update();

		// ���ظö����id ����ʧ�ܷ���-1
		int addPlayer(int userId);	// ����userId������Ŀ����Ϊ�˶�Ӧ��һ���û�����������ʱʹ�ã�

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

		// ������Ҫ�ṩһ���û�id��playerid��ӳ��
		// �����ԭ��������Ҫͬ�������û�����Ϣ���������ǲ��ܱ�֤�����û������пͻ��˾�����ͬ��playerId��
		// ���ǿ��Ա�֤���ǣ������û�����һ����ͬ��userId������������userIdȥӳ��playerId
		std::unordered_map<int, int>userMap;	
		List<Player> playerList;
		ObjectManager objManager;
		JoypadManager joypadManager;
	};

	System& staticSysyemHandle();

}