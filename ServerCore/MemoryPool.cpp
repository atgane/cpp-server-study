#include "pch.h"
#include "MemoryPool.h"

/*--------------------------------------
			MemoryPool
--------------------------------------*/

// 6. 메모리 풀은 allocSize를 받아 저장
MemoryPool::MemoryPool(int32 allocSize) : _allocSize(allocSize)
{
}

// 큐에 존재하는 모든 메모리를 할당 해제
MemoryPool::~MemoryPool()
{
	while (_queue.empty() == false)
	{
		MemoryHeader* header = _queue.front();
		_queue.pop();
		::free(header);
	}
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	WRITE_LOCK;
	ptr->allocSize = 0;
	
	// 8. queue에 다시 반납
	_queue.push(ptr);
	
	_allocCount.fetch_sub(1);
}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* header = nullptr;
	{
		WRITE_LOCK;
		if (_queue.empty() == false)
		{
			 // 7. 여분이 있으면 꺼내온다
			header = _queue.front();
			_queue.pop();
		}
	}

	// queue에 없는 경우
	if (header == nullptr)
	{
		header = reinterpret_cast<MemoryHeader*>(::malloc(_allocSize));
	}
	else
	{
		ASSERT_CRASH(header->allocSize == 0);
	}

	_allocCount.fetch_add(1);

	return header;
}
