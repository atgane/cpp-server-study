#include "pch.h"
#include "Allocator.h"
#include "Memory.h"

/*------------------------------------
			Base Allocater
------------------------------------*/

// 2. ���߿� �α׸� ��ų� �Ҵ��� ��ü�� ���ų�,
// ���� ����� �۾��� �� �� �ִ�. 

// �޸𸮸� �Ҵ�
void* BaseAllocator::Alloc(int32 size)
{
	return ::malloc(size);
}

// �޸𸮸� ����
void BaseAllocator::Release(void* ptr)
{
	::free(ptr);
}

/*-----------------------------------------
			StompAllocator
-----------------------------------------*/

// 2. ������ �ؼ� ����� �Ҵ��ش޶��ؼ� �׿� ���� ����� ������ ����� ����ش�.
void* StompAllocator::Alloc(int32 size)
{
	// 4����Ʈ��� 4 + 4095 = 4099�� �ǰ�, �̸� �ٽ� �����ָ� ��Ȯ�� 1���� ������ ī��Ʈ�� ��������. 
	// -1�� �ϴ� ������ size�� 4096�� ��츦 ����ϱ� �����̴�. 
	const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	
	// 5. �����÷ο� �غ��� ���� ������ ����
	const int64 dataOffset = pageCount * PAGE_SIZE - size;
	
	// 3. MEM_RESERVE | MEM_COMMIT: �޸𸮸� ����� ���ÿ� ����ϰڴٸ� ����. 
	void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	
	// 6.��ü ������ ����ϴ� ���� �ƴ� ������ �����¸�ŭ ���ؼ� �ش� �κи�ŭ �����Ѵ�. int8�� �ٲ�
	// ����Ʈ������ ũ�⸦ ��ȯ�Ͽ� offset��ŭ�� ����Ʈ�� address�� �����ֱ� �����̴�.
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