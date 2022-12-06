#pragma once

#include <mutex>

template<typename T>
class LockStack
{
public:
	LockStack() {}
	
	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_stack.push(std::move(value));
		
		// 4. ����ϰ� �ִ� ���� �ϳ� �����. 
		_condVar.notify_one();
	}

	// 2. �׷��� ���� ���θ� bool�� �����ϰ� �����͸� ���۷����� �������� �Ѵ�. 
	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty()) return false;

		// 2. ������ ��� C++���� top�� ������ pop�� �Ѵ�.
		value = std::move(_stack.top());
		_stack.pop();
		return true;
	}

	// 1. pop�� �� �� empty�� üũ�ϴ°� �ǹ̰� ����. �ֳĸ� �� ���� ������ �� �� �ֱ� ����.
	bool Empty()
	{
		lock_guard<mutex> lock(_mutex);
		return _stack.empty(); 
	}

	void WaitPop(T& value)
	{
		// 4. condition variable�� ��� unique lock�� �̿��Ѵ�. 
		unique_lock<mutex> lock(_mutex);
		// 5. lambda�� ����. stack�� ���Ұ� �ֱ� ������(=empty�� false�� ������. true������)�� ����Ѵ�. 
		_condVar.wait(lock, [this] { return _stack.empty() == false; });
		
		value = std::move(_stack.top());
		_stack.pop();
	}


private:
	stack<T> _stack;
	mutex _mutex;

	// 3. �����Ͱ� ���� ���, �����Ͱ� �ִ��� ������ üũ�� �� �����Ƿ� ��� ���ѷ����� ����
	// �˻縦 �ؾ� �� ���̴�. �̶� �츮�� condition variable�� �̿��Ͽ� ����ȭ�� �����ߴ�. 	
	condition_variable _condVar;
};