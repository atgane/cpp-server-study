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
#include "TypeCast.h"

class Class
{
public:
	virtual void Print() {

	}

private:
	int _hp = 0;
	int _mp = 0;
};

class Knight : public Class
{
private:
	int _hp = 0;
	int _mp = 0;
};

class Archer : public Class
{
private:
	int _hp = 0;
	int _mp = 0;
};

class Gunner : public Class
{
private:
	int _hp = 0;
	int _mp = 0;
};

class Mage : public Class
{
private:
	int _hp = 0;
	int _mp = 0;
};

int main() {
	using TL = TypeList<Class, Knight, Archer, Gunner, Mage>;
	
	TypeConversion<TL> test;

	test.s_convert[0][0];

	cout << endl;
}