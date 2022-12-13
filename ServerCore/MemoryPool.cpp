#include "pch.h"
#include "MemoryPool.h"

/*--------------------------------------
			MemoryPool
--------------------------------------*/

// 6. �޸� Ǯ�� allocSize�� �޾� ����
MemoryPool::MemoryPool(int32 allocSize) : _allocSize(allocSize)
{
}

// ť�� �����ϴ� ��� �޸𸮸� �Ҵ� ����
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
	
	// 8. queue�� �ٽ� �ݳ�
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
			 // 7. ������ ������ �����´�
			header = _queue.front();
			_queue.pop();
		}
	}

	// queue�� ���� ���
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
