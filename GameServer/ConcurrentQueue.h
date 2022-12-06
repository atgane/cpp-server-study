#pragma once

#include <mutex>

template<typename T>
class LockQueue
{
public:
	LockQueue() {}

	LockQueue(const LockQueue&) = delete;
	LockQueue& operator=(const LockQueue&) = delete;

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_queue.push(std::move(value));

		// 4. ����ϰ� �ִ� ���� �ϳ� �����. 
		_condVar.notify_one();
	}

	// 2. �׷��� ���� ���θ� bool�� �����ϰ� �����͸� ���۷����� �������� �Ѵ�. 
	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_queue.empty()) return false;

		// 2. ������ ��� C++���� top�� ������ pop�� �Ѵ�.
		value = std::move(_queue.front());
		_queue.pop();
		return true;
	}

	// 1. pop�� �� �� empty�� üũ�ϴ°� �ǹ̰� ����. �ֳĸ� �� ���� ������ �� �� �ֱ� ����.
	bool Empty()
	{
		lock_guard<mutex> lock(_mutex);
		return _queue.empty();
	}

	void WaitPop(T& value)
	{
		// 4. condition variable�� ��� unique lock�� �̿��Ѵ�. 
		unique_lock<mutex> lock(_mutex);
		// 5. lambda�� ����. queue�� ���Ұ� �ֱ� ������(=empty�� false�� ������. true������)�� ����Ѵ�. 
		_condVar.wait(lock, [this] { return _queue.empty() == false; });

		value = std::move(_queue.front());
		_queue.pop();
	}


private:
	queue<T> _queue;
	mutex _mutex;

	// 3. �����Ͱ� ���� ���, �����Ͱ� �ִ��� ������ üũ�� �� �����Ƿ� ��� ���ѷ����� ����
	// �˻縦 �ؾ� �� ���̴�. �̶� �츮�� condition variable�� �̿��Ͽ� ����ȭ�� �����ߴ�. 	
	condition_variable _condVar;
};