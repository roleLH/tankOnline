#pragma once
#include <vector>
#include <stack>
#include <iostream>
#include "util.h"


#define DEBUG

namespace tank_war
{
    enum EDir 
    {
        DIR_UP = 0,
        DIR_DOWN,
        DIR_LEFT,
        DIR_RIGHT
    };

    #define MAP_WIDTH 20
    #define MAP_HEIGHT 40

	struct SMap
	{
		int map[MAP_WIDTH][MAP_HEIGHT];
	};

	class Map
	{
	public:
		Map();
		void resetMap();
		inline SMap& getMap() { return map; }
	private:
		bool isEdit;
		// 关于地图，我们这样设计：第一个参数为横轴（x），第二个参数为纵轴（y）
		SMap map;
	};

	Map& staticMapHandle();
	


    class Object
    {
    public:
        Object();
        Object(int x, int y, int s, EDir d, bool l);
        virtual void update() = 0;
		inline bool live() { return isLive; }
		inline void birth() { isLive = true; }
		inline void die() { isLive = false; }
		inline int getX() const { return posX; }
		inline int getY() const { return posY; }
		inline char getDir() const 
		{ 
			switch (dir)
			{
				case EDir::DIR_UP: return 0x1e;
				case EDir::DIR_DOWN: return 0x1f;
				case EDir::DIR_LEFT: return 0x11;
				case EDir::DIR_RIGHT: return 0x10;
			}
		}
		inline bool getIsLive() const { return isLive; }
     protected:
        int posX, posY;
        int speed;
        EDir dir;
		bool isLive;
    };

	class Bullet : public Object
	{
	public:
		virtual void update();

		Bullet();
		Bullet(int x, int y, EDir d);
		void set(int x, int y, EDir d);
	private:
		static int bulletMap;
	};

    class Tank : public Object
    {
	public:
		Tank();

    public:
        virtual void update();
        void up();
        void down();
        void left();
        void right();
        void idle();
		Bullet fire();

        void clearOld();
        void buildNew();

    private:
        static int tankBmp[4][9];
		static int seed;
        int oldX, oldY;
        
    };



	class ObjectManager
	{
	public:
		void update();

		int newTank();
		Tank& getTank(const int id) { return tanks.getObject(id); }
		void freeTank(int id);

		int newBullet();
		Bullet& getBullet(const int id) { return bullets.getObject(id); }
		void freeBullet(int id);

	private:
		List<Tank> tanks;
		List<Bullet> bullets;
	};
}

