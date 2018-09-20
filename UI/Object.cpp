#include "Object.h"

namespace tank_war
{
	Map& staticMapHandle()
	{
		static Map staticMap;
		return staticMap;
	}

	Map::Map()
		: isEdit(false), map{}
	{	}

	void Map::resetMap()
	{
		for (size_t i = 0; i < MAP_WIDTH; ++i)
			for (size_t j = 0; j < MAP_HEIGHT; ++j)
				map.map[i][j] = 0;
	}
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

	// 一个非常简单的碰撞检测函数
	inline bool _easyCollision(Tank& tank, Bullet& bullet)
	{
		int tx = tank.getX(), ty = tank.getY();
		int bx = bullet.getX(), by = bullet.getY();
		//  点在矩形内
		return ((bx >= tx - 1 && bx <= tx + 1) && (by >= ty - 1 && by <= ty + 1));

	}

	int ObjectManager::easyCollision(int tankId)
	{
		Tank& t = tanks[tankId];
		int res = -1;
		for (size_t i{}; i < bullets.size(); ++i)
		{
			if (bullets[i].getIsLive() && bullets[i].getTankId() != tankId)
			{
				if (_easyCollision(t, bullets[i]))
				{
					res = i;
					break;
				}
			}
		}
		return res;
	}
}

namespace tank_war
{
	Object::Object(int x, int y, int s, EDir d, bool l)
		: posX(x), posY(y), speed(s), dir(d), isLive(l) { }

	// 这个默认构造函数好像有问题。
	Tank::Tank() : Object(Tank::seed, 1, 1, EDir::DIR_UP, false),
		oldX(Tank::seed), oldY(1), bulletCnt(1)
	{
		seed += 2;
	}

	int Tank::tankBmp[4][9] = {
		{ 0, 254, 0, 254, 254, 254, 254, 254, 254 },
		{ 254, 254, 254, 254, 254, 254, 0, 254, 0 },
		{ 0, 254, 254, 254, 254, 254, 0, 254, 254 },
		{ 254, 254, 0, 254, 254, 254, 254, 254, 0 }
	};
	int Tank::seed = 1;

	int Bullet::bulletMap = 254;

	void Tank::up()
	{
		int x = this->posX, y = this->posY;
		oldX = x;
		oldY = y;
		SMap& map = staticMapHandle().getMap();
		if (y + 2 > MAP_HEIGHT - 1)  return;
		if (this->dir == DIR_UP)
		{
			if (map.map[x - 1][y + 2] == 0 &&
				map.map[x][y + 2] == 0 &&
				map.map[x + 1][y + 2] == 0)
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
		if (y - 2 < 0) return;
		SMap& map = staticMapHandle().getMap();
		if (this->dir == DIR_DOWN)
		{
			if (map.map[x - 1][y - 2] == 0 &&
				map.map[x][y - 2] == 0 &&
				map.map[x + 1][y - 2] == 0)
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
		if (x - 2 < 0) return;
		SMap& map = staticMapHandle().getMap();
		if (this->dir == DIR_LEFT)
		{
			if (map.map[x - 2][y + 1] == 0 &&
				map.map[x - 2][y] == 0 &&
				map.map[x - 2][y - 1] == 0)
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
		if (x + 2 > MAP_WIDTH - 1) return;
		SMap& map = staticMapHandle().getMap();
		if (this->dir == DIR_RIGHT)
		{
			if (map.map[x + 2][y + 1] == 0 &&
				map.map[x + 2][y] == 0 &&
				map.map[x + 2][y - 1] == 0)
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
		SMap& map = staticMapHandle().getMap();

		map.map[x - 1][y + 1] = map.map[x][y + 1] = map.map[x + 1][y + 1] =
			map.map[x - 1][y] = map.map[x][y] = map.map[x + 1][y] =
			map.map[x - 1][y - 1] = map.map[x][y - 1] = map.map[x + 1][y - 1] = 0;

	}

	void Tank::buildNew()
	{
		int x = this->posX;
		int y = this->posY;
		int dir = static_cast<int>(this->dir);
		SMap& map = staticMapHandle().getMap();

		map.map[x - 1][y + 1] = tankBmp[dir][0];
		map.map[x][y + 1] = tankBmp[dir][1];
		map.map[x + 1][y + 1] = tankBmp[dir][2];
		map.map[x - 1][y] = tankBmp[dir][3];
		map.map[x][y] = tankBmp[dir][4];
		map.map[x + 1][y] = tankBmp[dir][5];
		map.map[x - 1][y - 1] = tankBmp[dir][6];
		map.map[x][y - 1] = tankBmp[dir][7];
		map.map[x + 1][y - 1] = tankBmp[dir][8];

	}

	void Tank::update()
	{
		clearOld();
		buildNew();

	}

	// 注意！！ 这个函数很烂，目的仅仅是为了不把system做成全局变量
	// 很烂的原因：我不喜欢这种复制。
	// 后记。但还是做成了全局变量。。。。
	bool Tank::fire()
	{
		if (bulletCnt > 0)
		{
			bulletCnt--;
			return true;
		}
		return false;
	}

	Bullet::Bullet()
		: Object(0, 0, 1, DIR_UP, false), playerId(-1)
	{	}

	Bullet::Bullet(int x, int y, EDir d)
		: Object(x, y, 2, d, true), playerId(-1)
	{	}

	void Bullet::set(int x, int y, EDir d, int t)
	{
		this->posX = x;
		this->posY = y;
		this->dir = d;
		this->playerId = t;
	}

	void Bullet::update()
	{
		int x = this->posX;
		int y = this->posY;
		SMap& map = staticMapHandle().getMap();
		map.map[x][y] = 0;
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
		map.map[x][y] = Bullet::bulletMap;
	}

}