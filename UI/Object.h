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

    
    static int map[MAP_WIDTH][MAP_HEIGHT];
    
	void resetMap()
	{
		for (size_t i = 0; i < MAP_WIDTH; ++i)
			for (size_t j = 0; j < MAP_HEIGHT; ++j)
				map[i][j] = 0;
	}

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


namespace tank_war
{
	int ObjectManager::newTank()
	{
		int tmp = tanks.newObject();
		if (tmp != -1)
			tanks[tmp].birth();
		return tmp;
	}
	void ObjectManager::freeTank(int id)
	{
		tanks[id].die();
		return tanks.freeObject(id);
	}

	int ObjectManager::newBullet()
	{
		int tmp = bullets.newObject();
		if (tmp != -1)
			bullets[tmp].birth();
		return tmp;
	}
	void ObjectManager::freeBullet(int id)
	{
		bullets[id].die();
		return bullets.freeObject(id);
	}

	void ObjectManager::update()
	{
		for (size_t i = 0; i < tanks.size(); ++i)
		{
			if (tanks[i].live())
				tanks[i].update();
		}
		for (size_t i = 0; i < bullets.size(); ++i)
		{
			if (bullets[i].live())
				bullets[i].update();
		}
	}

}

namespace tank_war 
{
	Object::Object(int x, int y, int s, EDir d, bool l) 
		: posX(x), posY(y), speed(s), dir(d), isLive(l) { }

	Tank::Tank() : Object(Tank::seed, 1, 1, EDir::DIR_UP, false), 
		oldX(Tank::seed), oldY(1)
	{
		seed += 2;
	}

    int Tank::tankBmp[4][9] = {
        {0, 254, 0, 254, 254, 254, 254, 254, 254},
        {254, 254, 254, 254, 254, 254, 0, 254, 0},
        {0, 254, 254, 254, 254, 254, 0, 254, 254},
        {254, 254, 0, 254, 254, 254, 254, 254, 0}
    };
	int Tank::seed = 1;

    int Bullet::bulletMap = 254;

    void Tank::up()
    {
        int x = this->posX, y = this->posY;
        oldX = x;
        oldY = y;
        if(y + 2 > MAP_HEIGHT - 1)  return;
        if(this->dir == DIR_UP)
        {
            if(map[x - 1][y + 2] == 0 && 
               map[x][y + 2] == 0 &&
               map[x + 1][y + 2] == 0)
               {
                   this->posY += this->speed;
               } 
        }
        else
        {
            this->dir = DIR_UP;
        }
    }

    void Tank::down()
    {
        int x = this->posX, y = this->posY;
        oldX = x;
        oldY = y;
        if(y - 2 < 0) return;
        if(this->dir == DIR_DOWN)
        {
            if(map[x - 1][y - 2] == 0 && 
               map[x][y - 2] == 0 &&
               map[x + 1][y - 2] == 0)
               {
                   this->posY -= this->speed;
               } 
        }
		else
		{
			this->dir = DIR_DOWN;
		}
    }

    void Tank::left()
    {
        int x = this->posX, y = this->posY;
        oldX = x;
        oldY = y;
        if(x - 2 < 0) return;
        if(this->dir == DIR_LEFT)
        {
            if(map[x - 2][y + 1] == 0 && 
               map[x - 2][y] == 0 &&
               map[x - 2][y - 1] == 0)
               {
                   this->posX -= this->speed;
               }
        }
		else
		{
			this->dir = DIR_LEFT;
		}
    }

    void Tank::right()
    {
        int x = this->posX, y = this->posY;
        oldX = x;
        oldY = y;
        if(x + 2 > MAP_WIDTH - 1) return;
        if(this->dir == DIR_RIGHT)
        {
            if(map[x + 2][y + 1] == 0 && 
               map[x + 2][y] == 0 &&
               map[x + 2][y - 1] == 0)
               {
                   this->posX += this->speed;
               }
        }
		else
		{
			this->dir = DIR_RIGHT;
		}
    }

    void Tank::idle()
    {
        
    }

    void Tank::clearOld()
    {
        int x = this->oldX;
        int y = this->oldY;

        map[x - 1][y + 1] = map[x][y + 1] = map[x + 1][y + 1] =
        map[x - 1][y] = map[x][y] = map[x + 1][y] =
        map[x - 1][y - 1] = map[x][y - 1] = map[x + 1][y - 1] = 0;
    
    }

    void Tank::buildNew()
    {
        int x = this->posX;
        int y = this->posY;
        int dir = static_cast<int>(this->dir);

        map[x - 1][y + 1]   = tankBmp[dir][0];
        map[x][y + 1]       = tankBmp[dir][1];
        map[x + 1][y + 1]   = tankBmp[dir][2];
        map[x - 1][y]       = tankBmp[dir][3];
        map[x][y]           = tankBmp[dir][4];
        map[x + 1][y]       = tankBmp[dir][5];
        map[x - 1][y - 1]   = tankBmp[dir][6];
        map[x][y - 1]       = tankBmp[dir][7];
        map[x + 1][y - 1]   = tankBmp[dir][8];

    }

    void Tank::update()
    {
        clearOld();
        buildNew();

    }

	// 注意！！ 这个函数很烂，目的仅仅是为了不把system做成全局变量
	// 很烂的原因：我不喜欢这种复制。
	Bullet Tank::fire()
	{
		return Bullet(this->posX, this->posY, this->dir);
	}

	Bullet::Bullet()
		: Object(0, 0, 1, DIR_UP, false)
	{	}

	Bullet::Bullet(int x, int y, EDir d)
		:	Object(x, y, 2, d, true)
	{	}

	void Bullet::set(int x, int y, EDir d)
	{
		this->posX = x;
		this->posY = y;
		this->dir = d;
	}

	void Bullet::update()
	{
		int x = this->posX;
		int y = this->posY;
		map[x][y] = 0;
		switch (this->dir)
		{
		case EDir::DIR_UP:
			if (y + this->speed > MAP_HEIGHT - 1)
				this->die();
			else
				y += this->speed;
			break;
		case EDir::DIR_DOWN:
			if (y - this->speed < 0)
				this->die();
			else
				y -= this->speed;
			break;
		case EDir::DIR_LEFT:
			if (x - this->speed < 0)
				this->die();
			else
				x -= this->speed;
			break;
		case EDir::DIR_RIGHT:
			if (x + this->speed > MAP_WIDTH - 1)
				this->die();
			else
				x += this->speed;
			break;
		}
		this->posX = x;
		this->posY = y;
		map[x][y] = Bullet::bulletMap;
	}

}