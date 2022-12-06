#pragma once

/*------------------------------------
			refCounable
------------------------------------*/

class RefCountable
{
public:
	RefCountable() : _refCount(1) {};
	// 1. 소멸자는 반드시 virtual(상속으로 전달하므로)
	virtual ~RefCountable() { };

	// 레퍼런스 카운트 리턴
	int32 GetRefCount() { return _refCount; }

	int32 AddRef() { return ++_refCount; }
	int32 ReleaseRef()
	{
		int32 refCount = --_refCount;
		
		// 아무도 레퍼런스를 해주지 않는다면 delete.
		if (refCount == 0)
		{
			delete this;
		}
		return refCount;
	}
	
protected:
	// 2. 싱글인 경우는 int32를 써도 된다. 그런데 멀티라면?
	// int32 _refCount;
	atomic<int32> _refCount; 
	// 이렇게 하면 끝나? 뒤질래?
	// RefCount는 아토믹하겠지만 외부에서 Count를 관리해야 하는데.
	// 안전하다는 보장을 할 수 있을까?
	// 응 못한다. 
};

// 3. 그래서 도입한다. 
// 스마트 포인터!
/*-------------------------------------------
				Smart Pointer
-------------------------------------------*/

// 스마트 포인터중 shared pointer를 구현해보자. 
// 이를 이용하면 직접 release나 set을 호출할 필요없이 사용할 수 있다.
// 이 개체가 이를 감쳐주어 대신한다. 
template<typename T> 
class TSharedPtr
{
public:
	TSharedPtr() { }
	// 변환 생성자로 생성하면 Set으로 자동으로
	// 레퍼런스 카운팅
	TSharedPtr(T* ptr) { Set(ptr); }
	
	// 복사. 상대방이 넘겨준 포인터를 나도 증가시킨다. 
	// 복사의 경우, 받아온 상대방의 포인터가 같이 관리해야 할 포인터가 된다. 
	TSharedPtr(const TSharedPtr& rhs) { Set(rhs._ptr); }
	// 이동.
	TSharedPtr(TSharedPtr&& rhs) { _ptr = rhs.ptr; rhs._ptr = nullptr; }
	// 상속 관계 복사
	template<typename U>
	TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._ptr)); }

	// 소멸자 호출시 자동으로 레퍼런스 카운팅을 줄이자. 
	~TSharedPtr() { Release(); }

	inline void Set(T* ptr)
	{
		// 포인터 초기화
		_ptr = ptr;
		if (ptr) {
			ptr->AddRef();
		}
	}

	inline void Release()
	{
		if (_ptr != nullptr)
		{
			_ptr->ReleaseRef();
			_ptr = nullptr;
		}
	}

public:
	// 복사 연산자
	TSharedPtr& operator=(const TSharedPtr& rhs)
	{
		if (_ptr != rhs._ptr)
		{
			Release();
			Set(rhs._ptr);
		}
		return *this;
	}

	TSharedPtr& operator=(TSharedPtr&& rhs)
	{
		// 기존에 가지고 있던 포인터를 release
		Release();
		// 새롭게 포인터 세팅
		_ptr = rhs._ptr;
		// 상대방 포인터 nullptr로 변경
		rhs._ptr = nullptr;
		return *this;
	}

	bool isNull() { return _ptr == nullptr; }

	bool operator==(const TSharedPtr& rhs) const { return _ptr == rhs._ptr; }
	bool operator==(T* ptr) const { return _ptr == ptr; }
	bool operator!=(const TSharedPtr& rhs) const { return _ptr != rhs._ptr; }
	bool operator!=(T* ptr) const { return _ptr != ptr; }
	bool operator<(const TSharedPtr& rhs) const { return _ptr < rhs._ptr; }
	// 포인터 연산자 오버로딩 
	T* operator*() { return _ptr; }
	const T* operator*() const { return _ptr; }
	T* operator->() { return _ptr; }
	const T* operator->() const { return _ptr; }
	operator T* () const { return _ptr; }

private:
	T* _ptr = nullptr;
};