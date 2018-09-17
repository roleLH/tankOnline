#include "Joypad.h"


namespace tank_war
{
	void Joypad::reset()
	{
		buttonMap[0] = buttonMap[1] =
			buttonMap[2] = buttonMap[3] =
			buttonMap[4] = buttonMap[5] = 0;
	}

	std::string Joypad::toStr()
	{
		std::string tmp = "000000";
		tmp[KEY_UP] = buttonMap[KEY_UP];
		tmp[KEY_DOWN] = buttonMap[KEY_DOWN];
		tmp[KEY_LEFT] = buttonMap[KEY_LEFT];
		tmp[KEY_RIGHT] = buttonMap[KEY_RIGHT];
		tmp[KEY_OK] = buttonMap[KEY_OK];
		tmp[KEY_CANCEL] = buttonMap[KEY_CANCEL];
		return tmp;
	}

	void Joypad::set(const char* data)
	{
		int i = 0;
		buttonMap[i++] = *data++;
		buttonMap[i++] = *data++;
		buttonMap[i++] = *data++;
		buttonMap[i++] = *data++;
		buttonMap[i++] = *data++;
		buttonMap[i++] = *data++;
	}
}
