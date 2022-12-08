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


/*-----------------------------------------
			StompAllocator
-----------------------------------------*/
// �޸� ���� ���׸� ã�� ���� Stomp Allocator�� ���� �������. 

class StompAllocator
{
	// 1. �Ҵ��� �� ������ ������ ������ �Ҵ��Ѵ�.
	enum { PAGE_SIZE = 0x1000 };

public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};