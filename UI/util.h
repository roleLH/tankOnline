#pragma once
#include <vector>
#include <stack>


template<typename Type>
class List
{
public:
	List(int size = 4) 
	{
		list.resize(size);
		for (int i = 0; i < size; ++i)
			freeList.push(i);
	}
	

	inline Type& getObject(const int id) { return list[id]; }
	inline Type& operator[] (const size_t id) { return list[id]; }
	inline size_t size() { return list.size(); }

	int newObject()
	{
		if (freeList.empty())
			return -1;
		int tmp = freeList.top();
		freeList.pop();
		return tmp;
	}
	inline void freeObject(int id)
	{
		freeList.push(id);
	}
private:
	std::vector<Type> list;
	std::stack<int> freeList;
};