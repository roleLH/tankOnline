#pragma once

#include <queue>
#include <vector>

namespace other
{
	enum ETypeOfCell
	{
		SPACE,
		STONE
	};

	enum EState
	{
		UNVISITED,
		CLOSED,
		OPEN
	};

	struct SCell
	{
		ETypeOfCell type;
		EState state;

		SCell() : type(ETypeOfCell::SPACE), state(EState::UNVISITED) {}
	};

	
	class CMap
	{
	public:

		inline SCell** getCells() const { return (SCell**)cells; }
		inline void setStone(const int x, const int y) { cells[x][y].type = ETypeOfCell::STONE; }
		inline void setSpace(const int x, const int y) { cells[x][y].type = ETypeOfCell::SPACE; }
	private:
		SCell cells[40][40];
	};

}

