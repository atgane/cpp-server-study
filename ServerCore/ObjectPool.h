#pragma once

#include "Types.h"
#include "MemoryPool.h"

template<typename Type>
class ObjectPool
{
public:
	template<typename... Args>
	static Type* Pop(Args&&... args)
	{
		// 2. 풀에서 해당 메모리를 꺼내고 헤더를 붙여서 생성자를 호출하고 뱉겠다
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(s_pool.Pop(), s_allocSize));
		new(memory)Type(forward<Args>(args)...);
		return memory;
	}
	static void Push(Type* obj)
	{
		obj->~Type();
		s_pool.Push(MemoryHeader::DetechHeader(obj));
	}
	static shared_ptr<Type> MakeShared()
	{
		shared_ptr<Type> ptr = { Pop(), Push };
		return ptr;
	}

private:
	// 
	static int32 s_allocSize;
	// 1. 타입 전용으로 메모리 풀을 만들어 사용한다. 
	static MemoryPool s_pool;
};

// static이여서 여기서 할당해주었다. 
template<typename Type>
int32 ObjectPool<Type>::s_allocSize = sizeof(Type) + sizeof(MemoryHeader);

template<typename Type>
MemoryPool ObjectPool<Type>::s_pool{ s_allocSize };