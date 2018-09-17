#pragma once
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