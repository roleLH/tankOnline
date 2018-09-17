#pragma once
#include <string>
#include <fstream>

namespace UI
{
	
	const std::string serverListTitle[] = {
		"              Server List",
		" server name          ip",
		"*-------------------------------------*",

	};

	class TankInfo
	{
	public:
		TankInfo(const char* file = "tank.bin");
		//~TankInfo();

		void setId(int id);
		void setPosition(int x, int y);
		// 0x10 right 0x11 left 0x1e up 0x1f down
		void setDirection(char dir);
		void setIsLive(bool live);
		void setScore(int score);
		void setJoyPad(const char* state);

		inline char* getInfoMap() const { return (char*)info; }

	private:
		void setInfo(int pos, std::string& buf);

		char info[400];
	};

	class GameUI
	{
	public:
		GameUI(const char* file = "ui.bin");
		void onChange(int selectId);
		void onChange2(int serverId);
		inline char* getBmp() const { return (char*)bmp; }
		inline char* getBmp2() const { return (char*)bmp2; }
	private:
		char bmp[400];
		char bmp2[400];
		char selectUI = 0x10;
	};

	static GameUI staticGameUI;
}
