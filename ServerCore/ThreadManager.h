#pragma once

#include <thread>
#include <functional>

/*--------------------------------
		ThreadManager
-------------------------------- */

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	// 4. 함수를 컬백을 하여 쓰레드를 실행
	void Launch(function<void(void)> callback);
	void Join();

	static void InitTLS();
	static void DestroyTLS();

private:
	Mutex _lock;
	vector<thread> _threads;
};

