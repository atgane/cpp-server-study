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
		
		// 4. 대기하고 있는 것을 하나 깨운다. 
		_condVar.notify_one();
	}

	// 2. 그래서 성공 여부를 bool로 리턴하고 데이터를 래퍼런스로 꺼내도록 한다. 
	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty()) return false;

		// 2. 보통의 경우 C++에서 top을 엿보고 pop을 한다.
		value = std::move(_stack.top());
		_stack.pop();
		return true;
	}

	// 1. pop을 할 때 empty를 체크하는건 의미가 없다. 왜냐면 그 사이 변수가 들어갈 수 있기 때문.
	bool Empty()
	{
		lock_guard<mutex> lock(_mutex);
		return _stack.empty(); 
	}

	void WaitPop(T& value)
	{
		// 4. condition variable의 경우 unique lock을 이용한다. 
		unique_lock<mutex> lock(_mutex);
		// 5. lambda를 쓰자. stack에 원소가 있기 전까지(=empty가 false일 때까지. true전까지)는 대기한다. 
		_condVar.wait(lock, [this] { return _stack.empty() == false; });
		
		value = std::move(_stack.top());
		_stack.pop();
	}


private:
	stack<T> _stack;
	mutex _mutex;

	// 3. 데이터가 없는 경우, 데이터가 있는지 없는지 체크할 수 없으므로 계속 무한루프를 돌며
	// 검사를 해야 할 것이다. 이때 우리는 condition variable을 이용하여 최적화를 진행했다. 	
	condition_variable _condVar;
};