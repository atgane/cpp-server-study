#pragma once
#include "Allocator.h"

// 3. 보편 참조. 이런 문법을 이용하면 가변 타입, 인자에 대해서
// 편리하게 메모리 할당을 사용할 수 있다.
template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	// 메모리 할당. static_cast를 사용한다 .Type의 크기만큼 메모리를 할당한다. 
	Type* memory = static_cast<Type*>(_xalloc(sizeof(Type)));
	
	// new~~가 없다면 생성자 호출이 일어나지 않고 메모리 할당만 일어난다. 
	// 이럴때 placement new를 이용한다.(new는 종류가 여러 개)
	new(memory)Type(forward<Args>(args)...);
	return memory;
}

// 오브젝트를 건네면 오브젝트를 release한다. 
template<typename Type>
void xdelete(Type* obj)
{
	// 오브젝트의 소멸자를 직접 호출한다. 
	obj->~Type();
	// 그 이후 메모리를 반납한다. 
	_xrelease(obj);
}