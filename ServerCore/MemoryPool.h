#pragma once

enum
{
	SLIST_ALIGNMENT = 16
};

/*--------------------------------------
			MemoryHeader
--------------------------------------*/

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{
	MemoryHeader(int32 size) : allocSize(size) {}

	static void* AttachHeader(MemoryHeader* header, int32 size) 
	{
		new(header)MemoryHeader(size);
		return reinterpret_cast<void*>(++header); // 따라서 이 리턴 값은 데이터가 시작되는 지점을 의미
	}

	static MemoryHeader* DetechHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}
	
	int32 allocSize;
};

/*--------------------------------------
			MemoryPool
--------------------------------------*/

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void Push(MemoryHeader* ptr);
	MemoryHeader* Pop();

private:
	int32 _allocSize = 0;
	atomic<int32> _useCount = 0;
	atomic<int32> _reserveCount = 0; // 메모리 풀에 저장이 된 것

	SLIST_HEADER _header; // lock-free stack을 관리하는 헤더
};

