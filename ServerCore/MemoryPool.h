#pragma once

/*--------------------------------------
			MemoryHeader
--------------------------------------*/

// 1. 메모리 풀을 여러개를 만든다. 
// 각각의 메모리 풀은 담당하는 크기를 갖는다. 동일한 크기끼리 관리한다. 
// 객체마다 사이즈가 다르기에 메모리 헤더를 만들어 줄 것이다. 

// 표준 메모리풀도 메모리 해더를 만들어서 관리한다.
struct MemoryHeader
{
	MemoryHeader(int32 size) : allocSize(size) {}

	static void* AttachHeader(MemoryHeader* header, int32 size) 
	{
		new(header)MemoryHeader(size);
		// 2. allocSize를 기입하고, 사용하는 클라이언트는 메모리 헤더에 대한 존재를 모르고,
		// 내부적으로 관리한다. header에 ++을 해주게 되면 메모리 헤더만큼 건너뛰게 되어 
		// 메모리 헤더 뒤의 데이터 시작점의 포인터를 알려주게 된다.
		return reinterpret_cast<void*>(++header); // 따라서 이 리턴 값은 데이터가 시작되는 지점을 의미
	}

	static MemoryHeader* DetechHeader(void* ptr)
	{
		// 3. 이 데이터의 시작위치를 넣어 메모리 해더를 반환
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}
	
	int32 allocSize;
	// TODO: 필요한 추가 정보
};

/*--------------------------------------
			MemoryPool
--------------------------------------*/

class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	// 5. 재사용하고 싶을 때 메모리에 집어넣거나 반납
	void Push(MemoryHeader* ptr);
	MemoryHeader* Pop();

private:
	// 4. 메모리 풀이 여러개가 있는데 각각 자신이 담당하는 사이즈를 명시
	int32 _allocSize = 0;
	// 메모리 풀에서 뱉어준 메모리의 개수를 의미
	atomic<int32> _allocCount = 0;

	// 락을 잡아서 잠시 들고있다 뱉을 수 있는 자료구조 하나를 선택한다
	USE_LOCK;
	queue<MemoryHeader*> _queue;
};

