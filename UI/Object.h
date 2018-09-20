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
		inline EDir getDirCode() const { return dir; }
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
		void set(int x, int y, EDir d, int playerId);

		inline void setplayerId(int t) { playerId = t; }
		inline int getPlayerId() const { return playerId; }
		inline void setTankId(int t) { tankId = t; }
		inline int getTankId() const { return tankId; }

	private:
		int playerId;	// 记录是哪一个`player`发出的
		int tankId;		// 同上。
		int groupId;	// 未使用
		static int bulletMap;
	};

    class Tank : public Object
    {
	public:
		Tank();

    public:
        virtual void update();

		// 添加此项的原因是，我们不能用playerid初始化位置坐标。
		inline void setPos(const int x, const int y)
		{
			posX = x; posY = y;
			oldX = x; oldY = y;
		}
		inline void IncBulletCnt() { bulletCnt++; }

        void up();
        void down();
        void left();
        void right();
        void idle();
		bool fire();	// 返回是否可以发射子弹

        void clearOld();
        void buildNew();

    private:
        static int tankBmp[4][9];
		static int seed;
        int oldX, oldY;
		int bulletCnt;		// 一次最多发射子弹的个数
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

		// 检测当前tank是否与某个子弹发生碰撞
		// 如果发生碰撞，返回该子弹id，否则返回-1
		int easyCollision(int tankId);
	private:
		List<Tank> tanks;
		List<Bullet> bullets;
	};
}

