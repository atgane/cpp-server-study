#pragma once

#include "Types.h"
#include "MemoryPool.h"
#include "Memory.h"

template<typename Type>
class ObjectPool
{
public:
	template<typename... Args>
	static Type* Pop(Args&&... args)
	{
#ifdef _STOMP
		MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(s_allocSize));
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(ptr, s_allocSize));
#else
		// 2. Ǯ���� �ش� �޸𸮸� ������ ����� �ٿ��� �����ڸ� ȣ���ϰ� ��ڴ�
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(s_pool.Pop(), s_allocSize));
#endif // _STOMP
		new(memory)Type(forward<Args>(args)...);
		return memory;
	}

	static void Push(Type* obj)
	{
		obj->~Type();
#ifdef _STOMP
		StompAllocator::Release(MemoryHeader::DetechHeader(obj));
#else 
		s_pool.Push(MemoryHeader::DetechHeader(obj));
#endif
	}

	static shared_ptr<Type> MakeShared()
	{
		shared_ptr<Type> ptr = { Pop(), Push };
		return ptr;
	}

private:
	// 
	static int32 s_allocSize;
	// 1. Ÿ�� �������� �޸� Ǯ�� ����� ����Ѵ�. 
	static MemoryPool s_pool;
};

// static�̿��� ���⼭ �Ҵ����־���. 
template<typename Type>
int32 ObjectPool<Type>::s_allocSize = sizeof(Type) + sizeof(MemoryHeader);

template<typename Type>
MemoryPool ObjectPool<Type>::s_pool{ s_allocSize };