#include "pch.h"
#include "CorePch.h"

#include <iostream>
#include <atomic>
#include <mutex>
#include <future>
#include <string>
#include <vector>
#include <thread>

#include "ThreadManager.h"
#include "RefCounting.h"
#include "memory.h"

class Player
{
public:
	Player() {}
	virtual ~Player() {}
};

class Knight : public Player
{
public:
	Knight()
	{
		cout << "K" << endl;
	}
	~Knight()
	{
		cout << "~K" << endl;
	}

	int32 _hp = 100;
	int32 _mp = 10;
};

int main() {
	Knight* knight = (Knight*)xnew<Player>();

	knight->_hp = 100;
	xdelete(knight);
}