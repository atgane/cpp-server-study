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

/*-----------------------------------------
			STL Allocator
-----------------------------------------*/

template<typename T>
class StlAllocator
{
public:
	using value_type = T;

	StlAllocator() {}

	template<typename Other>
	StlAllocator(const StlAllocator<Other>&) {}

	// ������ �Ҵ� �ż���. count�� ������ ����
	T* allocate(size_t count)
	{
		// �� �޸� ������
		const int32 size = static_cast<int32>(count * sizeof(T)); 
		return static_cast<T*>(_xalloc(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		_xrelease(ptr);
	}
};