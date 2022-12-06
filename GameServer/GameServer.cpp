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

using KnightRef = TSharedPtr<class Knight>;
using InventoryRef = TSharedPtr<class Inventory>;

class Knight : public RefCountable
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
	static void* operator new(size_t size)
	{
		cout << "new" << size << endl;
		void* ptr = ::malloc(size);
		return ptr;
	}

	static void operator delete(void* ptr)
	{
		cout << "delete" << endl;
		::free(ptr);
	}

public:

	KnightRef _target = nullptr;
	InventoryRef _inventory = nullptr;
};

class Inventory : public RefCountable
{
public:
	Inventory(KnightRef knight) : _knight(**knight)
	{

	}

	Knight& _knight;
};





int main() {
	Knight* knight = new Knight();
	delete knight;
}