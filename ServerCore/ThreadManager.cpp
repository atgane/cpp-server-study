#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

ThreadManager::ThreadManager()
{
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(function<void(void)> callback)
{
	// 5. �����带 ��´�. 
	LockGuard guard(_lock);

	// ���ٷ� ǥ�� �����带 �����Ų��. 
	// callback�� �ϴ� �Ϳ� TLS�� �ʱ�ȭ�ϰ� �����ִ� �κ��� �߰��Ѵ�.
	// ���� �Լ� �����͸� ���°ź��� std::function�� ���°� �̷Ӵ�.
	_threads.push_back(thread([=]()
		{
			InitTLS();
			callback();
			DestroyTLS();
		}));
}

void ThreadManager::Join()
{
	// 6. ��� �������� join�� ȣ���ϰ� �����ش�. 
	for (std::thread& t : _threads) {
		if (t.joinable())
			t.join();
	}
	_threads.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

// 7. �Ŀ� �������� ������ �����Ͱ� �ִٸ� ���⼭ ��������.
void ThreadManager::DestroyTLS()
{

}
