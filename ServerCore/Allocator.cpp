#include "pch.h"
#include "Allocator.h"

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