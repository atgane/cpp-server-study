#pragma once

/*------------------------------------
			Base Allocater
------------------------------------*/

// 1. ������ �Ҵ� ��å�� ���⼭ �����Ѵ�.
class BaseAllocator
{
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};

