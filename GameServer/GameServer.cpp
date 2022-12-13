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
	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Vector<Knight> v(10);
					
					Map<int32, Knight> a;
					
					a[100] = Knight();
					
					this_thread::sleep_for(10ms);
				}
			});
	}

	GThreadManager->Join();
}