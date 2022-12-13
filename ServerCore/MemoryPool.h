#pragma once

/*--------------------------------------
			MemoryHeader
--------------------------------------*/

// 1. �޸� Ǯ�� �������� �����. 
// ������ �޸� Ǯ�� ����ϴ� ũ�⸦ ���´�. ������ ũ�Ⳣ�� �����Ѵ�. 
// ��ü���� ����� �ٸ��⿡ �޸� ����� ����� �� ���̴�. 

// ǥ�� �޸�Ǯ�� �޸� �ش��� ���� �����Ѵ�.
struct MemoryHeader
{
	MemoryHeader(int32 size) : allocSize(size) {}

	static void* AttachHeader(MemoryHeader* header, int32 size) 
	{
		new(header)MemoryHeader(size);
		// 2. allocSize�� �����ϰ�, ����ϴ� Ŭ���̾�Ʈ�� �޸� ����� ���� ���縦 �𸣰�,
		// ���������� �����Ѵ�. header�� ++�� ���ְ� �Ǹ� �޸� �����ŭ �ǳʶٰ� �Ǿ� 
		// �޸� ��� ���� ������ �������� �����͸� �˷��ְ� �ȴ�.
		return reinterpret_cast<void*>(++header); // ���� �� ���� ���� �����Ͱ� ���۵Ǵ� ������ �ǹ�
	}

	static MemoryHeader* DetechHeader(void* ptr)
	{
		// 3. �� �������� ������ġ�� �־� �޸� �ش��� ��ȯ
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}
	
	int32 allocSize;
	// TODO: �ʿ��� �߰� ����
};

/*--------------------------------------
			MemoryPool
--------------------------------------*/

class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	// 5. �����ϰ� ���� �� �޸𸮿� ����ְų� �ݳ�
	void Push(MemoryHeader* ptr);
	MemoryHeader* Pop();

private:
	// 4. �޸� Ǯ�� �������� �ִµ� ���� �ڽ��� ����ϴ� ����� ���
	int32 _allocSize = 0;
	// �޸� Ǯ���� ����� �޸��� ������ �ǹ�
	atomic<int32> _allocCount = 0;

	// ���� ��Ƽ� ��� ����ִ� ���� �� �ִ� �ڷᱸ�� �ϳ��� �����Ѵ�
	USE_LOCK;
	queue<MemoryHeader*> _queue;
};

