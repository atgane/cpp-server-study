#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "Memory.h"

ThreadManager* GThreadManager = nullptr;
Memory* GMemory = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;

// 8. ���߿� ���� �޴������� ������ �����ϴ� ��� CoreGlobal����
// ������ �� �ֵ��� �Ѵ�.
// �̷��� �۾��ϸ� �Ŀ� main���� CoreGlobal Core�� �������� �����ϸ�
// �˾Ƽ� �Ŵ����� ȣ���Ѵ�.
class CoreGlobal
{
public:
	CoreGlobal() {
		GThreadManager = new ThreadManager();
		GMemory = new Memory();
		GDeadLockProfiler = new DeadLockProfiler();
	}
	~CoreGlobal() {
		delete GThreadManager;
		delete GMemory;
		delete GDeadLockProfiler;
	}
} GCoreGlobal;