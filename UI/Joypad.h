#pragma once
#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include "util.h"

namespace tank_war
{
    enum EButton
    {
        KEY_UP,
        KEY_DOWN,
        KEY_LEFT,
        KEY_RIGHT,
        KEY_OK,
        KEY_CANCEL
    };


    class Joypad
    {

    public:
        void reset();
		std::string toStr();
		void set(const char* data);
        inline bool isPress(EButton button) { return buttonMap[static_cast<int>(button)]; }
        inline void press(EButton button) { buttonMap[static_cast<int>(button)] = 1; }
    private:
        uint8_t buttonMap[6];
    };

    class JoypadManager 
    {
	public:
		
//		~JoypadManager();
		Joypad& getJoypad(const int id) { return list.getObject(id); }

		int newJoypad() { return list.newObject(); }
		void freeJoypadById(int id) { return list.freeObject(id); }

    private:
		List<Joypad> list;
    };


    
}


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
