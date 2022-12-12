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


/*-----------------------------------------
			StompAllocator
-----------------------------------------*/
// 메모리 관련 버그를 찾기 위해 Stomp Allocator를 만들어서 사용하자. 

class StompAllocator
{
	// 1. 할당할 때 페이지 사이즈 단위로 할당한다.
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

	// 데이터 할당 매서드. count는 원소의 개수
	T* allocate(size_t count)
	{
		// 총 메모리 사이즈
		const int32 size = static_cast<int32>(count * sizeof(T)); 
		return static_cast<T*>(_xalloc(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		_xrelease(ptr);
	}
};