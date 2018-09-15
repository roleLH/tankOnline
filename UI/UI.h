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

namespace UI
{
	TankInfo::TankInfo(const char* file)
	{
		std::fstream in(file, std::ios::binary | std::ios::in);
		if (!in.is_open())
			abort();
		in.read(info, 6);
		in.read(info, 400);
		in.close();
	}

	void TankInfo::setInfo(int pos, std::string& buf)
	{
		for (size_t i = 0; i < 9; ++i)
			info[pos + i] = 0;
		int size = buf.size();
		for (size_t i = pos + size; i > pos; --i)
		{
			info[i] = buf[pos + size - i];
		}
	}

	void i2cs(int num, std::string& buf)
	{
		while (num)
		{
			buf += static_cast<char>((num % 10) + '0');
			num /= 10;
		}
	}



	void TankInfo::setId(int id)
	{
		std::string buf = "";
		i2cs(id, buf);
		int size = buf.size();
		setInfo(49, buf);
	}

	void TankInfo::setPosition(int x, int y)
	{
		std::string buf = "]";
		i2cs(y, buf);
		buf += ',';
		i2cs(x, buf);
		buf += '[';
		setInfo(89, buf);
	}

	void TankInfo::setDirection(char dir)
	{
		std::string tmp = "";
		tmp += dir;
		setInfo(129, tmp);
	}

	void TankInfo::setIsLive(bool live)
	{
		std::string buf;
		if (live)
			buf = "eurt";
		else
			buf = "eslaf";
		setInfo(169, buf);
	}

	void TankInfo::setJoyPad(const char* state)
	{
		std::string buf = "";
		for (int i = 6; i >= 0; --i)
			buf += state[i];
		setInfo(369, buf);
	}


	GameUI::GameUI(const char* file)
	{
		std::fstream in(file, std::ios::binary | std::ios::in);
		if (!in.is_open())
			abort();
		in.read(bmp, 6);
		in.read(bmp, 400);
		in.read(bmp2, 400);
		in.close();
	}

	void GameUI::onChange(int selectId)
	{
		selectId %= 3;
		bmp[202] = bmp[242] = bmp[282] = 0;
		bmp[202 + selectId * 40] = selectUI;
	}

}