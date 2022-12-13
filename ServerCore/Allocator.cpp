#include "pch.h"
#include "Allocator.h"
#include "Memory.h"

/*------------------------------------
			Base Allocater
------------------------------------*/

// 2. 나중에 로그를 찍거나 할당한 객체를 보거나,
// 여러 디버깅 작업을 할 수 있다. 

// 메모리를 할당
void* BaseAllocator::Alloc(int32 size)
{
	return ::malloc(size);
}

// 메모리를 해제
void BaseAllocator::Release(void* ptr)
{
	::free(ptr);
}

/*-----------------------------------------
			StompAllocator
-----------------------------------------*/

// 2. 연산을 해서 사이즈를 할당해달라해서 그에 제일 가까운 페이지 사이즈를 뱉어준다.
void* StompAllocator::Alloc(int32 size)
{
	// 4바이트라면 4 + 4095 = 4099가 되고, 이를 다시 나눠주면 정확히 1개의 페이지 카운트가 떨어진다. 
	// -1을 하는 이유는 size가 4096인 경우를 대비하기 위함이다. 
	const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	
	// 5. 오버플로우 극복을 위한 오프셋 설정
	const int64 dataOffset = pageCount * PAGE_SIZE - size;
	
	// 3. MEM_RESERVE | MEM_COMMIT: 메모리를 예약과 동시에 사용하겠다를 선언. 
	void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	
	// 6.전체 영역을 사용하는 것이 아닌 데이터 오프셋만큼 더해서 해당 부분만큼 리턴한다. int8로 바꿔
	// 바이트단위로 크기를 변환하여 offset만큼의 바이트를 address에 더해주기 위함이다.
	return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
}

void StompAllocator::Release(void* ptr)
{
	const int64 address = reinterpret_cast<int64>(ptr);
	const int64 baseAddress = address - (address % PAGE_SIZE);
	
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}

/*-----------------------------------------
			PoolAllocator
-----------------------------------------*/

void* PoolAllocator::Alloc(int32 size)
{
	return GMemory->Allocate(size);
}

void PoolAllocator::Release(void* ptr)
{
	GMemory->Release(ptr);
}