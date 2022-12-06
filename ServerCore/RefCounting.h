#pragma once

/*------------------------------------
			refCounable
------------------------------------*/

class RefCountable
{
public:
	RefCountable() : _refCount(1) {};
	// 1. �Ҹ��ڴ� �ݵ�� virtual(������� �����ϹǷ�)
	virtual ~RefCountable() { };

	// ���۷��� ī��Ʈ ����
	int32 GetRefCount() { return _refCount; }

	int32 AddRef() { return ++_refCount; }
	int32 ReleaseRef()
	{
		int32 refCount = --_refCount;
		
		// �ƹ��� ���۷����� ������ �ʴ´ٸ� delete.
		if (refCount == 0)
		{
			delete this;
		}
		return refCount;
	}
	
protected:
	// 2. �̱��� ���� int32�� �ᵵ �ȴ�. �׷��� ��Ƽ���?
	// int32 _refCount;
	atomic<int32> _refCount; 
	// �̷��� �ϸ� ����? ������?
	// RefCount�� ������ϰ����� �ܺο��� Count�� �����ؾ� �ϴµ�.
	// �����ϴٴ� ������ �� �� ������?
	// �� ���Ѵ�. 
};

// 3. �׷��� �����Ѵ�. 
// ����Ʈ ������!
/*-------------------------------------------
				Smart Pointer
-------------------------------------------*/

// ����Ʈ �������� shared pointer�� �����غ���. 
// �̸� �̿��ϸ� ���� release�� set�� ȣ���� �ʿ���� ����� �� �ִ�.
// �� ��ü�� �̸� �����־� ����Ѵ�. 
template<typename T> 
class TSharedPtr
{
public:
	TSharedPtr() { }
	// ��ȯ �����ڷ� �����ϸ� Set���� �ڵ�����
	// ���۷��� ī����
	TSharedPtr(T* ptr) { Set(ptr); }
	
	// ����. ������ �Ѱ��� �����͸� ���� ������Ų��. 
	// ������ ���, �޾ƿ� ������ �����Ͱ� ���� �����ؾ� �� �����Ͱ� �ȴ�. 
	TSharedPtr(const TSharedPtr& rhs) { Set(rhs._ptr); }
	// �̵�.
	TSharedPtr(TSharedPtr&& rhs) { _ptr = rhs.ptr; rhs._ptr = nullptr; }
	// ��� ���� ����
	template<typename U>
	TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._ptr)); }

	// �Ҹ��� ȣ��� �ڵ����� ���۷��� ī������ ������. 
	~TSharedPtr() { Release(); }

	inline void Set(T* ptr)
	{
		// ������ �ʱ�ȭ
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
	// ���� ������
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
		// ������ ������ �ִ� �����͸� release
		Release();
		// ���Ӱ� ������ ����
		_ptr = rhs._ptr;
		// ���� ������ nullptr�� ����
		rhs._ptr = nullptr;
		return *this;
	}

	bool isNull() { return _ptr == nullptr; }

	bool operator==(const TSharedPtr& rhs) const { return _ptr == rhs._ptr; }
	bool operator==(T* ptr) const { return _ptr == ptr; }
	bool operator!=(const TSharedPtr& rhs) const { return _ptr != rhs._ptr; }
	bool operator!=(T* ptr) const { return _ptr != ptr; }
	bool operator<(const TSharedPtr& rhs) const { return _ptr < rhs._ptr; }
	// ������ ������ �����ε� 
	T* operator*() { return _ptr; }
	const T* operator*() const { return _ptr; }
	T* operator->() { return _ptr; }
	const T* operator->() const { return _ptr; }
	operator T* () const { return _ptr; }

private:
	T* _ptr = nullptr;
};