#include "pch.h"
#include "Allocator.h"

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