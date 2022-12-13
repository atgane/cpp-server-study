#include "pch.h"
#include "Memory.h"
#include "MemoryPool.h"

Memory::Memory()
{
	int32 size = 0;
	int32 tableIndex = 0;

	// 9. 32 사이즈로 1024까지 돌면서
	for (size = 32; size <= 1024; size += 32)
	{
		// 사이즈에 따라 메모리 풀을 만든다
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		// 테이블 인덱스를 돌면서, 0에서 32바이트까지 0번 풀을 참조하라고 세팅
		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (size = 1024; size <= 2048; size += 128)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (size = 2048; size <= 4096; size += 256)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}
}

Memory::~Memory()
{
	for (MemoryPool* pool : _pools)
		delete pool;
	
	_pools.clear();
}

void* Memory::Allocate(int32 size)
{
	MemoryHeader* header = nullptr;
	const int32 allocSize = size * sizeof(MemoryHeader);

	if (allocSize > MAX_ALLOC_SIZE)
	{
		// 메모리 풀링 최대 크기를 벗어나면 일반 할당
		header = reinterpret_cast<MemoryHeader*>(::malloc(allocSize));
	}
	else
	{
		// 메모리 풀에서 꺼내옴
		header = _poolTable[allocSize]->Pop();
	}

	// 헤더와 allocSize입력
	return MemoryHeader::AttachHeader(header, allocSize);
}

void Memory::Release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetechHeader(ptr);
	
	const int32 allocSize = header->allocSize;
	ASSERT_CRASH(allocSize > 0);

	if (allocSize > MAX_ALLOC_SIZE)
	{
		// 메모리 풀링 최대 크기를 벗어나면 일반 해제
		::free(header);
	}
	else
	{
		// 메모리 풀에 메모리 반납
		_poolTable[allocSize]->Push(header);
	}
}
