#pragma once

/*------------------------------------
			Base Allocater
------------------------------------*/

// 1. 앞으로 할당 정책을 여기서 정의한다.
class BaseAllocator
{
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};

