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
#include "Allocator.h"

class Knight
{
public:
	int32 _hp = rand() % 1000;
};

class Monster
{
public:
	int64 _id = 0;
};

int main() {
	Knight* k = ObjectPool<Knight>::Pop();
	ObjectPool<Knight>::Push(k);

	shared_ptr<Knight> sptr = { ObjectPool<Knight>::Pop(), ObjectPool<Knight>::Push };

	
	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([]() {
			while (true)
			{
				Knight* knight = xnew<Knight>();
				cout << knight->_hp << endl;
				this_thread::sleep_for(10ms);
				xdelete(knight);
			}
		});
	}

	GThreadManager->Join();
}