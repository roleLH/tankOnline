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

