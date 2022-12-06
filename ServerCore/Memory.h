#pragma once
#include "Allocator.h"

// 3. ���� ����. �̷� ������ �̿��ϸ� ���� Ÿ��, ���ڿ� ���ؼ�
// ���ϰ� �޸� �Ҵ��� ����� �� �ִ�.
template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	// �޸� �Ҵ�. static_cast�� ����Ѵ� .Type�� ũ�⸸ŭ �޸𸮸� �Ҵ��Ѵ�. 
	Type* memory = static_cast<Type*>(_xalloc(sizeof(Type)));
	
	// new~~�� ���ٸ� ������ ȣ���� �Ͼ�� �ʰ� �޸� �Ҵ縸 �Ͼ��. 
	// �̷��� placement new�� �̿��Ѵ�.(new�� ������ ���� ��)
	new(memory)Type(forward<Args>(args)...);
	return memory;
}

// ������Ʈ�� �ǳ׸� ������Ʈ�� release�Ѵ�. 
template<typename Type>
void xdelete(Type* obj)
{
	// ������Ʈ�� �Ҹ��ڸ� ���� ȣ���Ѵ�. 
	obj->~Type();
	// �� ���� �޸𸮸� �ݳ��Ѵ�. 
	_xrelease(obj);
}