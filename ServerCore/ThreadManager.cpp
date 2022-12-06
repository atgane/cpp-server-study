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
	// 5. 락가드를 잡는다. 
	LockGuard guard(_lock);

	// 람다로 표준 스레드를 실행시킨다. 
	// callback을 하는 것에 TLS를 초기화하고 날려주는 부분을 추가한다.
	// 또한 함수 포인터를 쓰는거보다 std::function을 쓰는게 이롭다.
	_threads.push_back(thread([=]()
		{
			InitTLS();
			callback();
			DestroyTLS();
		}));
}

void ThreadManager::Join()
{
	// 6. 모든 스레드의 join을 호출하고 지워준다. 
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

// 7. 후에 동적으로 생성된 데이터가 있다면 여기서 날려주자.
void ThreadManager::DestroyTLS()
{

}
